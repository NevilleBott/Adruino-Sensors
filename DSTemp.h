// =================================================
// DS Temp Sensor
// =================================================

#include <Arduino.h>
#include <stdio.h>
#include "sensor.h"
#include <OneWire.h>

#define maxstatsmsglen  40
#define maxnamelen  10

class DSTemp : public sensor , public OneWire
{
public:

  byte sensorPin;       //= 21;
  
        DSTemp(char *name,float ifvalue,float ifconst,float minv,float maxv, boolean ienabled, boolean debug, int pin) : sensor(name, ifvalue, ifconst, minv, maxv, ienabled, debug) , OneWire(pin)
        {
        };
        
        
        void getTemp();
        
   private:

 };  

