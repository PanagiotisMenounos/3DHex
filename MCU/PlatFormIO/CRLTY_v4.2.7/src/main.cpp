#include <Arduino.h>

HardwareSerial myserial(PA10, PA9);

void setup() {
  myserial.begin(115200); 
}

void loop() {
  myserial.println("Hello World!");
  delay(1000);
}