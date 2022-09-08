![GitHub release (latest by date)](https://img.shields.io/github/v/release/TheFidax/digitalPinFast)
![GitHub Release Date](https://img.shields.io/github/release-date/TheFidax/digitalPinFast?color=blue&label=last%20release)
![GitHub commits since latest release (by date)](https://img.shields.io/github/commits-since/TheFidax/digitalPinFast/latest?color=orange)
[![arduino-library-badge](https://www.ardu-badge.com/badge/digitalPinFast.svg)](https://www.ardu-badge.com/digitalPinFast)
[![License](https://img.shields.io/github/license/TheFidax/digitalPinFast)](#)

# Indice
* [Introduzione](#Fast-Digital-Pin)
* [Inizializzazione](#Inizializzazione)
* [Metodi Disponibili](#Metodi-Disponibili)
* [Distruzione](#Distruzione)

------------

# Fast Digital Pin
Questa libreria e' pensata per quelle applicazione dove *un numero limitato* di pin vengono usati con *molta frequenza*, pertanto velocizzare il loro utilizzo diventa molto importante.

La libreria permette di velocizzare il comando dei pin digitali (pinMode, digitalWrite, digitalRead) andando a calcolare, **una sola volta** al momento della creazione dell'Oggetto i parametri richiesti (porta, bit, registri) per i pin su cui si vogliono utilizzare le *funzioni fast*.

**ATTENZIONE: Questa velocizzazione viene ottenuta ospitando in SRAM l'oggetto contenente i dati del pin; *ogni Oggetto pesa 6 Byte!*** 

I meotdi forniti dalla libreria **non disattivano gli interrupt e non eseguono i controlli sui Timer/PWM**, pertanto il suo utilizzo e' pensato per utenti *piu' esperti*.</br>

**Video Presentazione del Progetto:**</br>

[![Video Presentazione](https://img.youtube.com/vi/xxxxxxx/0.jpg)](http://www.youtube.com/watch?v=xxxxx)

------------

# Inizializzazione
L'inizializzazione dell'oggetto avviene al momento della sua Creazione

------------

```c
digitalPinFast(uint8_t pinNumber);
```
Dichiarazione dell'Oggetto digitalPinFast, l'argomento da passare e' il numero del pin sul quale creare l'oggetto.
**Deve essere un pin Digital**.

------------

# Metodi Disponibili

I seguenti metodi sono forniti dalla libreria per sostituire i comandi di *default*.

------------

```c
void pinModeFast(uint8_t mode)
```
Analogo a pinMode, evita controlli su pin PWM e NON disabilita gli interrupt.</br>
- Input
  - la nuova configurazione del pin
- Restituisce:
  - nulla

------------

```c
uint8_t digitalReadFast(void);
```
Analogo a digitalRead, evita controlli su pin PWM e NON disabilita gli interrupt.</br>
- Input:
  - nulla
- Restituisce:
  - il valore logico letto sul pin

------------

```c
void digitalWriteFast(uint8_t val);
```
Analogo a digitalWrite, evita controlli su pin PWM e NON disabilita gli interrupt.</br>
- Input:
  - il nuovo stato del pin
- Restituisce:
  - nulla

------------

```c
void setNewFastPin(uint8_t pinNumber);
```
Serve a modificare il pin a cui e' collegato l'oggetto.
- Input:
  - il numero del nuovo pin su cui si vogliono utilizzare i metodi Fast
- Restituisce:
  - nulla

------------

# Distruzione
L'oggetto *puo'* essere distrutto.

```c
~digitalPinFast(void);
```

Al momento della distruzione **il pin viene messo nello stato di INPUT per evitare danneggiamenti accidentali**.
