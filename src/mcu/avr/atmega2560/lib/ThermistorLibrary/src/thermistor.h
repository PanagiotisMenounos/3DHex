
#ifndef THERMISTOR_H
  #define THERMISTOR_H


class thermistor
{
  public:
    thermistor(int pin, int sensorNumber);
    float analog2temp();
    float analog2tempEADC(int adcValue);
    int _pin;///15.5.2022 change 3dhex
    int _sensorNumber;///15.5.2022 change 3dhex
  protected:
};



#endif
