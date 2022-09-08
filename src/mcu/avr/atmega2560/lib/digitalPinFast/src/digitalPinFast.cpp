#include "digitalPinFast.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void digitalPinFast::determine_pinData(uint8_t pinNumber) {
    if (pinNumber < NUM_DIGITAL_PINS) {
        _pinBitMask = digitalPinToBitMask(pinNumber);
        _pinPort = digitalPinToPort(pinNumber);
        _pinReg = portModeRegister(_pinPort);
        _pinOut = portOutputRegister(_pinPort);
    }
    else {
        determine_pinData(LED_BUILTIN);
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

digitalPinFast::digitalPinFast(uint8_t pinNumber) {
    determine_pinData(pinNumber);
}

digitalPinFast::~digitalPinFast(void) {
    pinModeFast(INPUT);
    
    _pinReg = NULL;
    _pinOut = NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
 
void digitalPinFast::setNewFastPin(uint8_t pinNumber) {
    determine_pinData(pinNumber);
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void digitalPinFast::pinModeFast(uint8_t mode) {
    if (mode == INPUT) {
        *_pinReg &= ~_pinBitMask;
        *_pinOut &= ~_pinBitMask;
    }
    else if (mode == INPUT_PULLUP) {
        *_pinReg &= ~_pinBitMask;
        *_pinOut |= _pinBitMask;
    }
    else {
        *_pinReg |= _pinBitMask;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t digitalPinFast::digitalReadFast(void) {
    if (*portInputRegister(_pinPort) & (_pinBitMask)) {
        return HIGH;
    }

    return LOW;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void digitalPinFast::digitalWriteFast(uint8_t val) {
    if (val == LOW) {
        *_pinOut &= ~_pinBitMask;
    }
    else {
        *_pinOut |= _pinBitMask;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
