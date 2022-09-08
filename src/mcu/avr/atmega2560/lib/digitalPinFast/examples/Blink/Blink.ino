#include <digitalPinFast.h>

digitalPinFast pinLed(LED_BUILTIN);

void setup() {
    pinLed.pinModeFast(OUTPUT);
}

// the loop function runs over and over again forever
void loop() {
    pinLed.digitalWriteFast(HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    pinLed.digitalWriteFast(LOW);    // turn the LED off by making the voltage LOW
    delay(1000);                       // wait for a second
}
