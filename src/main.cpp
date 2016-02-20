#include "Arduino.h"
#include <IRremoteESP8266.h>
#include <IRremoteInt.h>

const int VOL_UP = 0x490;
const int VOL_DOWN = 0xc90;
const int ARROW_UP = 0x385d;

int RECV_PIN = D2;

IRrecv irrecv(RECV_PIN);

decode_results results;

void setup() {
  Serial.begin(115200);
  irrecv.enableIRIn();
  Serial.println("Listening for Sony beams");
}

void loop() {
  irrecv.decode(&results);
  if (irrecv.decode(&results)) {
    if(results.decode_type == SONY) {
      Serial.println(results.value, HEX);
      switch(results.value) {
        case VOL_UP:
          Serial.println("VOL UP");
          break;
        case ARROW_UP:
          Serial.println("ARROW UP");
          break;
        case VOL_DOWN:
          Serial.println("VOL DOWN");
          break;
      }
    }
    irrecv.resume();
  }
  delay(350);
}
