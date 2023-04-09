#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>

#define trigger D0
#define LED D2
#define power D3

void setup()
{
  Serial.begin(9600);

  pinMode(trigger, INPUT);

  pinMode(LED, OUTPUT);

  pinMode(power, OUTPUT);

  if (digitalRead(trigger) == HIGH) {

    digitalWrite(power, HIGH);

    WiFiManager wifiManager;

    wifiManager.resetSettings();

    wifiManager.autoConnect("CIRCUIT DIGEST WiFi Manager");

    Serial.println("connected :)");
  }
}

void loop()
{

  if (WiFi.status() == WL_CONNECTED) {

    digitalWrite(power, LOW);

    while (WiFi.status() == WL_CONNECTED) {

      digitalWrite(LED, HIGH);

      delay(500);

      digitalWrite(LED, LOW);

      delay(200);
    }
  } else {

    digitalWrite(LED, LOW);
  }
}