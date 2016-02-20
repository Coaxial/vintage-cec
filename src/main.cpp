#include <Arduino.h>
#include <IRremoteESP8266.h>
#include <IRremoteInt.h>
#include <ESP8266WiFi.h>

#include "secrets.h"

#define DEBUG false

const int VOL_UP = 0x490;
const int VOL_DOWN = 0xc90;
const int ARROW_UP = 0x385d;

const int IR_PIN = D2;

IRrecv irrecv(IR_PIN);
decode_results results;

// TODO clean variables etc

void setupDebugConsole() {
  Serial1.begin(115200);
  Serial1.setDebugOutput(DEBUG);
  Serial1.println();
  Serial1.println("Debug console ready");
}

void connectWiFi() {
  Serial1.print("Connecting to WiFi ");
  Serial1.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASSWORD);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  // TODO handle failure to connect

  Serial1.print("WiFi connected with IP: ");
  Serial1.println(WiFi.localIP());
}

void sendCommand(WiFiClient client, const char* host) {
  delay(100); // Give the MCU some time to run WiFi and TCP tasks
  String url = "/";
  url += "?millisecs=";
  url += millis();
  
  Serial1.print("Requesting URL: ");
  Serial1.println(url);
  
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  Serial1.println("success");
}

void connectToHost(const char* host, const int port) {
  WiFiClient client;
  // TODO use a websocket
  Serial1.println();
  Serial1.println();
  Serial1.print("connecting to ");
  Serial1.println(host);
  
  delay(100); // Give the MCU some time to run WiFi and TCP tasks
  const int httpPort = port;
  if (!client.connect(host, httpPort)) {
    Serial1.println("connection failed");
    return;
  }

  sendCommand(client, host);
}

void listenForIR() {
  irrecv.enableIRIn();
}

void setup() {
  setupDebugConsole();
  connectWiFi();
  listenForIR();
}

void loop() {
  irrecv.decode(&results);
  if (irrecv.decode(&results)) {
    if(results.decode_type == SONY) {
      Serial1.println(results.value, HEX);
      switch(results.value) {
        case VOL_UP:
          Serial1.println("VOL UP");
          connectToHost(KODI_HOST, KODI_PORT);
          break;
        case ARROW_UP:
          Serial1.println("ARROW UP");
          break;
        case VOL_DOWN:
          Serial1.println("VOL DOWN");
          break;
      }
    }
    irrecv.resume();
  }
  // TODO implement a real debounce
  delay(350);
}
