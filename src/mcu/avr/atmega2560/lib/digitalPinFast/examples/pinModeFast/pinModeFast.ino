#include <digitalPinFast.h>

#define PIN_TEST 4

void setup() {
  digitalPinFast pinTest(PIN_TEST);
  unsigned long initTime, endTime;
  
  Serial.begin(115200);
  while(!Serial) {}
  
  Serial.println("FastDigitalPin test: pinModeFast");
  Serial.print("sizeof digitalPinFast Object: "); Serial.println(sizeof(pinTest));
  Serial.println();

  pinMode(PIN_TEST, INPUT);

  Serial.println("Arduino pinMode() function:");
  initTime = micros();
  pinMode(PIN_TEST, OUTPUT);
  endTime = micros();

  Serial.print("initTime : "); Serial.println(initTime);
  Serial.print("endTime : "); Serial.println(endTime);
  Serial.print("Time : "); Serial.println(endTime - initTime);
  Serial.println();

  pinMode(PIN_TEST, INPUT);
  delay(2000);

  Serial.println("pinModeFast() function:");
  initTime = micros();
  pinTest.pinModeFast(OUTPUT);
  endTime = micros();

  Serial.print("initTime : "); Serial.println(initTime);
  Serial.print("endTime : "); Serial.println(endTime);
  Serial.print("Time : "); Serial.println(endTime - initTime);
  Serial.println();

  pinMode(PIN_TEST, INPUT);
  delay(2000);

  Serial.println("register function:"); //D4 = PD4 
  initTime = micros();
  DDRD = B00010000;
  endTime = micros();

  Serial.print("initTime : "); Serial.println(initTime);
  Serial.print("endTime : "); Serial.println(endTime);
  Serial.print("Time : "); Serial.println(endTime - initTime);
  Serial.println();

  pinMode(PIN_TEST, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:

}
