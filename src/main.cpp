#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LittleFS.h>
#include <Adafruit_NeoPixel.h>

#include "IClock.h"
#include "ArduinoClock.h"
#include "IIntensitySensor.h"
#include "VL53L0XIntensitySensor.h"
#include "HeartbeatReader.h"

// Led Ring config
#define LED_RING_DATA_PIN D5
#define LED_RING_NUM_LEDS 24
#define LED_RING_TYPE WS2812B
#define LED_RING_COLOR_ORDER GRB
#define LED_RING_VOLTS 5
#define LED_RING_MAX_MA 4000

// Pressure sensor config
#define PRESSURE_SENSOR_PIN A0

// MQTT Broker Connection
const char *mqtt_broker_host = MQTT_BROKER_HOST;
const int mqtt_port = MQTT_BROKER_PORT;
const char *mqtt_username = MQTT_USERNAME;
const char *mqtt_password = MQTT_PASSWORD;

// Wifi connection
X509List mqtt_broker_certificate;
WiFiManager wifiManager;
WiFiClientSecure espClient;

// MQTT event bus
PubSubClient client(espClient);

// Heartbeat reader
IClock *clockService = new ArduinoClock();
IIntensitySensor *intensitySensorService = new VL53L0XIntensitySensor();
HeartbeatReader heartbeatReader(clockService, intensitySensorService);

// Led ring
Adafruit_NeoPixel strip = Adafruit_NeoPixel(LED_RING_NUM_LEDS, LED_RING_DATA_PIN, NEO_GRB + NEO_KHZ800);

//

void setup_led_ring()
{
  strip.begin();
  strip.setBrightness(225);
  strip.show();
}

void setup_devices()
{
  // Initialise VL53L0X sensor
  intensitySensorService->setup();

  if (!intensitySensorService->isAvailable())
  {
    Serial.println(F("Error initialising VL53L0X"));
    while (1)
      ;
  }
}

void setup_wifi_connection()
{
  wifiManager.resetSettings();
  wifiManager.autoConnect("LoverTheAir - Device 1");

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }

  Serial.println("Connected to wifi!");
}

void setup_client_certificate()
{
  if (!LittleFS.begin())
  {
    Serial.println("An Error has occurred while initializing the Filesystem");
    return;
  }

  File file = LittleFS.open("/emqxsl-ca.crt", "r");
  if (!file)
  {
    Serial.println("Failed to open cretificate file");
    return;
  }

  mqtt_broker_certificate.append(file.readString().c_str());
  espClient.setTrustAnchors(&mqtt_broker_certificate);
  file.close();
}

void setup_time_ntp_server()
{
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

  Serial.print("Waiting for NTP time sync: ");
  time_t now = time(nullptr);
  while (now < 8 * 3600 * 2)
  {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
  }
  Serial.println("");
  struct tm timeinfo;
  gmtime_r(&now, &timeinfo);
  Serial.print("Current time: ");
  Serial.print(asctime(&timeinfo));
}

void handle_mqtt_event(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived in topic: ");
  Serial.println(topic);
  Serial.print("Message:");
  for (unsigned int i = 0; i < length; i++)
  {
    Serial.print((char)payload[i]);
  }
  Serial.println();
  Serial.println("-----------------------");
}

void setup_mqtt_broker_connection()
{
  client.setServer(mqtt_broker_host, mqtt_port);
  client.setCallback(handle_mqtt_event);

  while (!client.connected())
  {
    String client_id = "love-node-";
    client_id += String(WiFi.macAddress());
    Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password))
    {
      Serial.println("Public emqx mqtt broker connected");
    }
    else
    {
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }
}

void setup()
{
  Serial.begin(115200);

  setup_devices();
  setup_led_ring();
  setup_wifi_connection();
  setup_client_certificate();
  setup_time_ntp_server();
  setup_mqtt_broker_connection();

  // publish and subscribe
  client.publish("heartbeat", "hello bitches!");
  client.subscribe("heartbeat");

  // Subscribe to heartbeats
  heartbeatReader.setNewHeartbeatCallback([](Heartbeat *heartbeat) {    
    std::vector<Pulse> pulses = heartbeat->pulses();

    // TODO This should be controlled by the HeartBeat
    if (pulses.size() <= 1) {
      return;
    }

    Serial.println("Heartbeat -----");
    for(Pulse pulse : pulses) {
      Serial.print("P -> i");
      Serial.print(pulse.intensity);
      Serial.print(", t");
      Serial.println(pulse.time);
    }
    Serial.println("-----");
  });
}

uint16_t read_pressure () {
  uint16_t pressure = analogRead(PRESSURE_SENSOR_PIN);
  uint16_t constrainedPressure = constrain(pressure, 20, 90);

  return map(constrainedPressure, 20, 90, 0, 255);
}

void loop()
{
  uint16_t intensity = intensitySensorService->read();
  uint16_t pressure = read_pressure();

  Serial.print("Pressure: ");
  Serial.println(pressure);

  Serial.print("Intensity: ");
  Serial.println(intensity);

  Serial.print("Timestamp: ");
  Serial.println(clockService->milliseconds());

  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, strip.Color(228, 121, pressure));
  }

  strip.setBrightness(intensity);
  strip.show();

  heartbeatReader.loop();

  if (WiFi.status() == WL_CONNECTED)
  {
    client.loop();
  }
}