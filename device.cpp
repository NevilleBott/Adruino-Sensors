//
//
//
//

#include "device.h"

device::device(char *name,unsigned long ontime,unsigned long offtime,int pinno, boolean enabled, boolean stat) {
        _Pin = pinno;
        pinMode(_Pin, OUTPUT);
        strcpy(_name,name);
        _ontime = ontime;
        _offtime = offtime;

	_enabled = enabled;
        if (_enabled ) {
          strcpy(_statusmsg,"Enabled");
          Control(stat);
        }
        else {
          strcpy(_statusmsg,"Disabled");
          Control(false);
        }  
}

// =================================================
// Control
// =================================================
void device::Control(boolean __on) {
  if (_enabled ) {
      if (__on) {
          strcpy(_statusmsg,"ON");
          digitalWrite(_Pin, HIGH);               // On
          _starttime = millis()/1000;  
          _on = true;        
           Serial.println("ON");
      } else {
          strcpy(_statusmsg,"OFF");
          digitalWrite(_Pin, LOW);               // Off
          _stoptime = millis()/1000;  
          _on = false;
           Serial.println("Off");
      }
  } else {
    _starttime = 0;
    _stoptime = 0;
  }  
}
// =================================================
// Timer Remaining
// =================================================
char *device::TimeToGo() {
   int ttg;
   if ( _enabled ) {
       unsigned long secs = millis() /1000;
       if (_on) {
           ttg = int(_ontime - secs + _starttime);
       } else {
           ttg = int(_offtime - secs + _stoptime);
       } 
       String s1 = String(ttg);   
       s1.toCharArray(chrs,5);
   //Serial.println(s1);
   } else {
       strcpy(chrs,"Off"); 
   }  
   return chrs;
     
}

// =================================================
// CSV for UDP
// =================================================

char *device::toCSV() {
    strcpy(_strbuf,_name);
    strcat(_strbuf,",");
    
    String s1 = String(int(_ontime));   
    s1.toCharArray(chrs,5);
    strcat(_strbuf,chrs);
    strcat(_strbuf,",");

    s1 = String(int(_offtime));   
    s1.toCharArray(chrs,5);
    strcat(_strbuf,chrs);
    strcat(_strbuf,",");
    
    strcat(_strbuf,_statusmsg);
    strcat(_strbuf,",");
    
    strcat(_strbuf,TimeToGo());          
    
    return _strbuf;
}

// =================================================
// XML Time Remaining function
// =================================================
char *device::toXMLStringTime() {
      strcpy(_strbuf,"<");
      strcat(_strbuf,_name);
      strcat(_strbuf,"Time");
      strcat(_strbuf,">");

      strcat(_strbuf,TimeToGo());
      strcat(_strbuf," s");
      
      strcat(_strbuf,"</");
      strcat(_strbuf,_name);
      strcat(_strbuf,"Time");
      strcat(_strbuf,">");
      return _strbuf;        
}
// =================================================
// Timer Mode
// =================================================
boolean device::CheckTime() {
    unsigned long secs = millis() /1000;
   // Pump running for set time stop the pump
   if (_on) {
       if ((secs - _starttime) > _ontime) {
           Control(false);
       } 
       return false;
   } else {
       if ( (secs - _stoptime) > _offtime) {
           Control(true);
           return true;
       } else {
         return false;
       }       
   }
}

// =================================================
// Control from Serial or UDP
// =================================================
void device::command(String comstring) {
   
    Serial.print("Received Command for ");
    Serial.println(_name);
   
    unsigned long iNewVal = 0;
   
    if (comstring.startsWith("ONT")) {      
      iNewVal = comstring.substring(3).toInt();   
      Serial.println("On Time Set To");
      Serial.println(iNewVal);
      OnTime(iNewVal); 
    }  
    else if (comstring.startsWith("OFFT")) {      
      iNewVal = comstring.substring(4).toInt();   
      Serial.println("Off Time Set To");
      Serial.println(iNewVal);
      OffTime(iNewVal); 
    }  

     else if (comstring.startsWith("ON")) {
         Control(true);               // On
     }
     else if (comstring.startsWith("OFF")) {
         Control(false);               // Off
     }
     else if (comstring.startsWith("EN")) {
         Enable(true);              
     }
     else if (comstring.startsWith("DIS")) {
         Enable(false);      
     }
     else if (comstring.startsWith("DB")) {
         Debug(comstring.substring(2).toInt());
     }
}
