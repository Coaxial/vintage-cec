#include "Arduino.h"
#include <IRremoteESP8266.h>
#include <IRremoteInt.h>
#include <ESP8266WiFi.h>

#include "secrets.h"

const int VOL_UP = 0x490;
const int VOL_DOWN = 0xc90;
const int ARROW_UP = 0x385d;

int RECV_PIN = D2;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setupDebugConsole() {
  Serial1.begin(115200);
  Serial1.setDebugOutput(true);
  Serial1.println("");
  Serial1.println("Debug console ready");
}

void connectWiFi() {
  Serial1.print("Connecting to ");
  Serial1.println(wlan_ssid);

  WiFi.begin(wlan_ssid, wlan_password);

  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial1.print('.');
  }

  Serial1.println("");
  Serial1.print("Wifi connected with IP: ");
  Serial1.println(WiFi.localIP());
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
  delay(350);
}
