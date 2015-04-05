//
//
//
//

#include "sensor.h"


sensor::sensor(char *name,float ifvalue,float ifconst,float minv,float maxv, boolean ienabled, boolean debug)
{
        strcpy(_name,name);
	fvalue = ifvalue;
	fconst = ifconst;
        _minv = minv;
        _maxv = maxv;  
	enabled = ienabled;
        calibrated = true;
        _debugenabled = debug;
        if (enabled ) {
          strcpy(_statusmsg,"Not Ready");
        }
        else {
          strcpy(_statusmsg,"Disabled");
        }  
}

//
// Update value and filtered value
//
boolean sensor::newival(float newval)
{
	ivalue = newval;
 
        if ((ivalue >= _minv) && ( ivalue <= _maxv)) {
	  fvalue = (ivalue  - fvalue) * fconst + fvalue;	// Update filtered value
          if (calibrated) {
            statusmsg("Ok");
          } else {
             statusmsg("Ok No T Cal");
          }
        
          if ( _debugenabled) { 
              Serial.println(_name); 
              Serial.println("Value");
              Serial.println(ivalue, 1); 
              Serial.println("Filtered");
              Serial.println(fvalue, 1); 
          }  
          return true;
        }
        else
        {
          statusmsg("Out of Range");
          if ( _debugenabled) { 
              Serial.println(_name); 
              Serial.println("Out of Range"); 
          } 
          return false;
        }  
}

//
// change to inline ?????
// 
boolean sensor::filteredinrange() {
        if ((fvalue >= _minv) && ( fvalue <= _maxv)) {
          return true;
        }
        else
        {
          return false;
        }   
}

// =================================================
// Control from Serial or UDP
// =================================================
void sensor::command(String comstring) {

   Serial.print("Received Command for ");
   Serial.println(_name);

   if (comstring.startsWith("EN")) {
     Enable(true);              
   }
   else if (comstring.startsWith("DIS")) {
     Enable(false);      
   }
   else if (comstring.startsWith("DB")) {
     Debug(comstring.substring(2).toInt());
   }
   else if (comstring.startsWith("K")) { 
      Serial.println("Set K From");
      Serial.println(getfconst());
      Serial.println("Set K To");
      setfconst(float(comstring.substring(1).toInt()) / 10000);
      Serial.println(getfconst());
    }
}
