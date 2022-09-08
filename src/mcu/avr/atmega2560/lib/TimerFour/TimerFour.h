/*
 *  Interrupt and PWM utilities for 16 bit Timer4 on ATmega2560
 *  Original code by Jesse Tane for http://labs.ideo.com August 2008
 *  Modified March 2009 by Jérôme Despatis and Jesse Tane for ATmega328 support
 *  Modified June 2009 by Michael Polli and Jesse Tane to fix a bug in setPeriod() which caused the timer to stop
 *  Modified April 2012 by Paul Stoffregen - portable to other AVR chips, use inline functions
 *  Modified again, June 2014 by Paul Stoffregen - support Teensy 3.1 & even more AVR chips
 *  Modified February 2016 by Vincent Limorté, porting Timer3 to Timer4 for ATmega2560. Removed teensy support, as i don't own it to test
 *
 *  This is free software. You can redistribute it and/or modify it under
 *  the terms of Creative Commons Attribution 3.0 United States License. 
 *  To view a copy of this license, visit http://creativecommons.org/licenses/by/3.0/us/ 
 *  or send a letter to Creative Commons, 171 Second Street, Suite 300, San Francisco, California, 94105, USA.
 *
 */

#ifndef TimerFour_h_
#define TimerFour_h_

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "config/timer_four_pins.h"

#define TIMER4_RESOLUTION 65536UL  // Timer4 is 16 bit


// Placing nearly all the code in this .h file allows the functions to be
// inlined by the compiler.  In the very common case with constant values
// the compiler will perform all calculations and simply write constants
// to the hardware registers (for example, setPeriod).


class TimerFour
{


#if defined(__AVR__)
  public:
    //****************************
    //  Configuration
    //****************************
    void initialize(unsigned long microseconds=1000000) __attribute__((always_inline)) {
	TCCR4B = _BV(WGM43);        // set mode as phase and frequency correct pwm, stop the timer
	TCCR4A = 0;                 // clear control register A
	setPeriod(microseconds);
    }
    void setPeriod(unsigned long microseconds) __attribute__((always_inline)) {
    	/* WGM43 mode goes from BOTTOM to TOP (ICR4) then from TOP to BOTTOM, then from BOTTOM to TOP.
    	 * OVF interrupt is triggered on TOP, thus the period covers 2*ICR4 counts.
    	 * Max period is 8'388'480 µs
    	 */
    	const unsigned long cycles = (F_CPU / 2000000) * microseconds;
	if (cycles < TIMER4_RESOLUTION) {
		clockSelectBits = _BV(CS40);
		pwmPeriod = cycles;
	} else
	if (cycles < TIMER4_RESOLUTION * 8) {
		clockSelectBits = _BV(CS41);
		pwmPeriod = cycles / 8;
	} else
	if (cycles < TIMER4_RESOLUTION * 64) {
		clockSelectBits = _BV(CS41) | _BV(CS40);
		pwmPeriod = cycles / 64;
	} else
	if (cycles < TIMER4_RESOLUTION * 256) {
		clockSelectBits = _BV(CS42);
		pwmPeriod = cycles / 256;
	} else
	if (cycles < TIMER4_RESOLUTION * 1024) {
		clockSelectBits = _BV(CS42) | _BV(CS40);
		pwmPeriod = cycles / 1024;
	} else {
		clockSelectBits = _BV(CS42) | _BV(CS40);
		pwmPeriod = TIMER4_RESOLUTION - 1;
	}
	ICR4 = pwmPeriod;
	TCCR4B = _BV(WGM43) | clockSelectBits;
    }

    //****************************
    //  Run Control
    //****************************
    void start() __attribute__((always_inline)) {
	TCCR4B = 0;
	TCNT4 = 0;		// TODO: does this cause an undesired interrupt?
	resume();
    }
    void stop() __attribute__((always_inline)) {
	TCCR4B = _BV(WGM43);
    }
    void restart() __attribute__((always_inline)) {
	start();
    }
    void resume() __attribute__((always_inline)) {
	TCCR4B = _BV(WGM43) | clockSelectBits;
    }

    //****************************
    //  PWM outputs
    //****************************
    /*
     * Duty goes from 0 to 1023.
     */
    void setPwmDuty(char pin, unsigned int duty) __attribute__((always_inline)) {
	unsigned long dutyCycle = pwmPeriod;
	dutyCycle *= duty;
	dutyCycle >>= 10;
	if (pin == TIMER4_A_PIN) OCR4A = dutyCycle;
	#ifdef TIMER4_B_PIN
	else if (pin == TIMER4_B_PIN) OCR4B = dutyCycle;
	#endif
	#ifdef TIMER4_C_PIN
	else if (pin == TIMER4_C_PIN) OCR4C = dutyCycle;
	#endif
    }
    void pwm(char pin, unsigned int duty) __attribute__((always_inline)) {
	if (pin == TIMER4_A_PIN) { pinMode(TIMER4_A_PIN, OUTPUT); TCCR4A |= _BV(COM4A1); }
	#ifdef TIMER4_B_PIN
	else if (pin == TIMER4_B_PIN) { pinMode(TIMER4_B_PIN, OUTPUT); TCCR4A |= _BV(COM4B1); }
	#endif
	#ifdef TIMER4_C_PIN
	else if (pin == TIMER4_C_PIN) { pinMode(TIMER4_C_PIN, OUTPUT); TCCR4A |= _BV(COM4C1); }
	#endif
	setPwmDuty(pin, duty);
	TCCR4B = _BV(WGM43) | clockSelectBits;
    }
    void pwm(char pin, unsigned int duty, unsigned long microseconds) __attribute__((always_inline)) {
	if (microseconds > 0) setPeriod(microseconds);
	pwm(pin, duty);
    }
    void disablePwm(char pin) __attribute__((always_inline)) {
	if (pin == TIMER4_A_PIN) TCCR4A &= ~_BV(COM4A1);
	#ifdef TIMER4_B_PIN
	else if (pin == TIMER4_B_PIN) TCCR4A &= ~_BV(COM4B1);
	#endif
	#ifdef TIMER4_C_PIN
	else if (pin == TIMER4_C_PIN) TCCR4A &= ~_BV(COM4C1);
	#endif
    }

    //****************************
    //  Interrupt Function
    //****************************
    void attachInterrupt(void (*isr)()) __attribute__((always_inline)) {
	isrCallback = isr;
	TIMSK4 = _BV(TOIE4);
    }
    void attachInterrupt(void (*isr)(), unsigned long microseconds) __attribute__((always_inline)) {
	if(microseconds > 0) setPeriod(microseconds);
	attachInterrupt(isr);
    }
    void detachInterrupt() __attribute__((always_inline)) {
	TIMSK4 = 0;
    }
    static void (*isrCallback)();

  private:
    // properties
    static unsigned short pwmPeriod;
    static unsigned char clockSelectBits;

#endif
};

extern TimerFour Timer4;

#endif

