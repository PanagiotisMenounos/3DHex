#ifndef DIGITAL_PIN_FAST_h
#define DIGITAL_PIN_FAST_h

#include <stdint.h>
#include "arduino.h"
#include "pins_arduino.h"

class digitalPinFast {
private:
    uint8_t _pinPort;            // Identifica la Porta del pin
    uint8_t _pinBitMask;         // Identifica il bit del pin nel registro: PB0 -> porta PB, bit0
    volatile uint8_t* _pinReg;   // Identifica il registro del pin
    volatile uint8_t* _pinOut;

    /*
    *	determine_pinData() ricava le informazioni sul pin passato tramite le macro native e le salva nelle variabili private
    *	Input:
    *		- il numero del pin di cui ricavare le informazioni
    *       - la struct dove salvare i dati
    *	Restituisce:
    *		- nulla
    */
    void determine_pinData(uint8_t pinNumber);

public:
    /*  
    *   digitalPinFast() dichiarazione dell'oggetto digitalPinFast
    *   Input:
    *       - il numero del pin su cui si vuole creare l'oggetto
    */
    digitalPinFast(uint8_t pinNumber);
    /*  
    *   ~digitalPinFast() distruzione dell'Oggetto
    *   Input:
    *       - Nulla
    *   Restituisce:
    *       - Nulla
    */
    ~digitalPinFast(void);
    /*
    *   setNewFastPin() permette di assegnare un nuovo pin ad un oggetto gia' esistente
    *   Input:
    *       - il numero del pin su cui si vuole creare l'oggetto
    *	Restituisce:
    *		- nulla
    */
    void setNewFastPin(uint8_t pinNumber);
    /*
    *	FastPinMode() analogo a pinMode, evita controlli su pin PWM e NON disabilita gli interrupt
    *	Input:
    *       - la nuova configurazione del pin (INPUT, INPUT_PULLUP, OUTPUT)
    *	Restituisce:
    *		- nulla
    */
    void pinModeFast(uint8_t mode);
    /*
    *	FastDigitalRead() analogo a digitalRead, evita controlli su pin PWM e NON disabilita gli interrupt
    *	Input:
    *		- nulla
    *	Restituisce:
    *		- il valore logico letto sul pin
    */
    uint8_t digitalReadFast(void);
    /*
    *	FastDigitalWrite() analogo a digitalWrite, evita controlli su pin PWM e NON disabilita gli interrupt
    *	Input:
    *       - il nuovo stato del pin (HIGH, LOW)
    *	Restituisce:
    *		- nulla
    */
    void digitalWriteFast(uint8_t val);
};

#endif
