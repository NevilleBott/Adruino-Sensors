// =================================================
// Flow Meter
// =================================================

#include <Arduino.h>
#include <stdio.h>
#include "sensor.h"

#define maxstatsmsglen  40
#define maxnamelen  10

class FlowMeter : public sensor
{
public:

  byte sensorInterrupt; //= 2;  // 0 = digital pin 21
  byte sensorPin;       //= 21;
  
        FlowMeter(char *name,float ifvalue,float ifconst,float minv,float maxv, boolean ienabled, boolean debug,int pin,int interuptno) : sensor(name, ifvalue, ifconst, minv, maxv, ienabled, debug) 
        {
              sensorInterrupt = interuptno;  // 2
              sensorPin       = pin;    //21;
              calibrationFactor = 5.5;
          
              pinMode(sensorPin, INPUT);
              digitalWrite(sensorPin, HIGH);
          
              pulseCount        = 0;
              flowRate          = 0.0;
              flowMilliLitres   = 0;
              prevMilliLitres   = 0;
              totalMilliLitres  = 0;
              oldTime           = 0;
              
              _firsttotal = true;
        };

        void incpulseCount();
        
        //void pulseCounter();
        inline void ResetTotal() { 
          Serial.println("Flow Reset "); 
          Serial.println(totalMilliLitres,0); 
          prevMilliLitres = totalMilliLitres; 
          totalMilliLitres = 0 ; 
          _firsttotal = false;
          _intenabled = false;
        };
          
          // to CSV
	inline char *toCSV() {
          // Beging Packet can block so read flow here after beginpacket has returned  
        //  ReadFlow();
          strcpy(_strbuf,getname());
          strcat(_strbuf,",");
 
          dtostrf(ivalue, 6, 3, _sx);
          strcat(_strbuf,_sx);
          strcat(_strbuf,",");

          sprintf(_sx, "%lu", prevMilliLitres);
          strcat(_strbuf,_sx);
          strcat(_strbuf,",");
   
          strcat(_strbuf,getstatusmsg());          
          return _strbuf;
        };
  
        inline void ReadFlow() {
          
                    //Serial.print("S8");
                    // Disable the interrupt while calculating flow rate and sending the value to
                // the host
                //detachInterrupt(sensorInterrupt);
              //  _intenabled = false;  
                 pulseraw = pulseCount; 
                // Reset the pulse counter so we can start incrementing again
                
                 if ( _debugenabled) { 
                   Serial.println("Flow Meter Read Flow");
                   Serial.println(pulseraw);
                 }   
     
                 pulseCount = 0;
                
                // Because this loop may not complete in exactly 1 second intervals we calculate
                // the number of milliseconds that have passed since the last execution and use
                // that to scale the output. We also apply the calibrationFactor to scale the output
                // based on the number of pulses per second per units of measure (litres/minute in
                // this case) coming from the sensor.
                flowRate = ((1000.0 / float((millis() - oldTime))) * float(pulseraw)) / calibrationFactor;
                
                // Note the time this processing pass was executed. Note that because we've
                // disabled interrupts the millis() function won't actually be incrementing right
                // at this point, but it will still return the value it was set to just before
                // interrupts went away.
                oldTime = millis();
                
                // Divide the flow rate in litres/minute by 60 to determine how many litres have
                // passed through the sensor in this 1 second interval, then multiply by 1000 to
                // convert to millilitres.
                flowMilliLitres = (flowRate / 60) * 1000;
                
                // Add the millilitres passed in this second to the cumulative total
                totalMilliLitres += flowMilliLitres;
                
                if ( _firsttotal) { prevMilliLitres = totalMilliLitres; };
             //   unsigned int frac;
                
                // Print the flow rate for this second in litres / minute
               // Serial.print("Flow rate: ");
               // Serial.print(int(flowRate));  // Print the integer part of the variable
             //  Serial.print(".");             // Print the decimal point
                // Determine the fractional part. The 10 multiplier gives us 1 decimal place.
                //frac = (flowRate - int(flowRate)) * 10;
             //   Serial.print(frac, DEC) ;      // Print the fractional part of the variable
            //   Serial.print("L/min");
                // Print the number of litres flowed in this second
            //    Serial.print("  Current Liquid Flowing: ");             // Output separator
            //    Serial.print(flowMilliLitres);
            //    Serial.print("mL/Sec");
            
                // Print the cumulative total of litres flowed since starting
            //   Serial.print("  Output Liquid Quantity: ");             // Output separator
            //     Serial.print(totalMilliLitres);
             //   Serial.println("mL"); 
            
                newival(flowRate);
                
                
                // Enable the interrupt again now that we've finished sending output
               // attachInterrupt(sensorInterrupt, pulseCounter, FALLING);
               _intenabled = true;
          };
        
  private:

    volatile byte pulseCount;  
    byte pulseraw;
    // The hall-effect flow sensor outputs approximately 5.5 pulses per second per
    // litre/minute of flow.
    float calibrationFactor; // = 5.5;
    float flowRate;
    unsigned int flowMilliLitres;
    unsigned long totalMilliLitres;
    unsigned long prevMilliLitres;
    unsigned long oldTime;
    char _strbuf[maxnamelen+1 + 16 + maxstatsmsglen+1];
    char _sx[7];
    boolean _firsttotal;
    boolean _intenabled;
};  

