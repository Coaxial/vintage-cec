#include "Arduino.h"
#include <IRremoteESP8266.h>
#include <IRremoteInt.h>
#include <ESP8266WiFi.h>

#include "secrets.h"

const int VOL_UP = 0x490;
const int VOL_DOWN = 0xc90;
const int ARROW_UP = 0x385d;

const int IR_PIN = D2;

IRrecv irrecv(IR_PIN);

decode_results results;

// TODO clean variables etc

void setupDebugConsole() {
  Serial1.begin(115200);
  Serial1.setDebugOutput(true);
  Serial1.println();
  Serial1.println("Debug console ready");
}

void connectWiFi() {
  Serial1.print("Connecting to WiFi ");
  Serial1.println(wlan_ssid);

  WiFi.begin(wlan_ssid, wlan_password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print(".");
  }
  // TODO handle failure to connect

  Serial1.print("WiFi connected with IP: ");
  Serial1.println(WiFi.localIP());
}

void testHTTPGet() {
  // TODO use a websocket
  long func_start = millis();
  Serial1.println();
  Serial1.println();
  Serial1.print("connecting to ");
  Serial1.println(target_host);
  
  // Use WiFiClient class to create TCP connections
  long wicl_start = millis();
  WiFiClient client;
  long wicl_end = millis();
  // Give the MCU some time to run WiFi and TCP tasks
  delay(100);
  const int httpPort = 8000;
  long clco_start = millis();
  if (!client.connect(target_host, httpPort)) {
    Serial1.println("connection failed");
    return;
  }
  long clco_end = millis();
  
  // We now create a URI for the request
  String url = "/";
  url += "?microsecs=";
  url += micros();
  
  Serial1.print("Requesting URL: ");
  Serial1.println(url);
  
  // This will send the request to the server
  long clpr_start = millis();
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + target_host + "\r\n" + 
               "Connection: close\r\n\r\n");
  long clpr_end = millis();
  // Give the MCU some time to run WiFi and TCP tasks
  delay(100);
  long func_end = millis();

  Serial1.println();
  Serial1.println("closing connection");
  Serial1.println("Timers: ");
  Serial1.print("  instantiate WiFiClient: ");
  Serial1.print(wicl_end - wicl_start);
  Serial1.println("ms");
  Serial1.print("  client connect: ");
  Serial1.print(clco_end - clco_start);
  Serial1.println("ms");
  Serial1.print("  send request: ");
  Serial1.print(clpr_end - clpr_start);
  Serial1.println("ms");
  Serial1.print("  whole function: ");
  Serial1.print(func_end - func_start);
  Serial1.println("ms");
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
          testHTTPGet();
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
