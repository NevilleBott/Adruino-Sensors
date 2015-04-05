//
//
//
//

#include "sensor.h"
#include "DSTemp.h"


// =================================================
//
// One Wire Temprature Read
//
//
// =================================================
void DSTemp::getTemp(){
  //returns the temperature from one DS18S20 in DEG Celsius

  byte data[12];
  byte addr[8];

  if ( !search(addr)) {
    //no more sensors on chain, reset search
    reset_search();
    statusmsg("Search Reset");
  }

  else if ( OneWire::crc8( addr, 7) != addr[7]) {
    statusmsg("CRC is not valid!");
  }

  else if ( addr[0] != 0x10 && addr[0] != 0x28) {
    statusmsg("Device is not recognized");
  }
  else { 
      reset();
      select(addr);
      write(0x44,1); // start conversion, with parasite power on at the end
    
     // delay(750);     // maybe 750ms is enough, maybe not  
    
      byte present = reset();
      select(addr);    
      write(0xBE); // Read Scratchpad
    
    
      for (int i = 0; i < 9; i++) { // we need 9 bytes
        data[i] = read();
      }
    
      reset_search();
    
      
        byte MSB = data[1];
        byte LSB = data[0];
        
       if (MSB != 0 && LSB != 0 ) {
         
       float tempRead = ((MSB << 8) | LSB); //using two's compliment
        float TemperatureSum = tempRead / 16;
        newival(TemperatureSum);
      }
  }
}
