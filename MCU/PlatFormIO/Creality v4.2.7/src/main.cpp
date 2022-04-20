#include <Arduino.h>

HardwareSerial Serial1(PA10, PA9);

void setup() {
  Serial1.begin(115200); 
}

void loop() {
  Serial1.println("Hello World!");
  delay(1000);
}
