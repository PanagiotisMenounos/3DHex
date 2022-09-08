/**
 * 2019 Vincent Limorté
 * Test and sample using Timer4 lib.
 * Set a PWM pulse, with 1000000 microsec period, and 256/1024 payload.
 * Led is on 1/4 second, and off for 3/4 second
 */

#include <TimerFour.h>

void setup()
{
	// PWM pins attached to Timer4 are Mega pins 6, 7, 8
	// TIMER4_A_PIN is 6
	// TIMER4_B_PIN is 7
	// TIMER4_C_PIN is 8 which is used in the example
	Timer4.initialize(1000000);
	Timer4.pwm(TIMER4_C_PIN, 256);
}

void loop()
{
	// nothing
}
