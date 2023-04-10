#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <LittleFS.h>
#include <Adafruit_VL53L0X.h>

// Pinout
#define LED D4

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

// Laser sensor
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

void setup_devices()
{
  // Initialise LED
  pinMode(LED, OUTPUT);

  // Initialise VL53L0X sensor
  if (!lox.begin())
  {
    Serial.println(F("Error initialising VL53L0X"));
    while (1)
      ;
  }
}

void led_blink(int repetitions, int time_interval)
{
  for (int i = 0; i < repetitions; i++)
  {
    digitalWrite(LED, HIGH);
    delay(time_interval);
    digitalWrite(LED, LOW);
    delay(time_interval);
  }
}

void measure_distance()
{
  VL53L0X_RangingMeasurementData_t measure;

  lox.rangingTest(&measure, false);

  if (measure.RangeStatus != 4)
  {
    Serial.print("Distance (mm): ");
    Serial.println(measure.RangeMilliMeter);
  }
  else
  {
    Serial.println("Out of range");
  }

  delay(100);
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

  led_blink(4, 200);
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
  setup_wifi_connection();
  setup_client_certificate();
  setup_time_ntp_server();
  setup_mqtt_broker_connection();

  // publish and subscribe
  client.publish("heartbeat", "hello bitches!");
  client.subscribe("heartbeat");
}

void loop()
{
  if (WiFi.status() == WL_CONNECTED)
  {
    digitalWrite(LED, LOW);

    measure_distance();

    client.loop();
  }
  else
  {
    led_blink(1, 1000);
  }
}