// =================================================
//
// Level Read
//
//
// =================================================
float Level_UpdateValue() {

	// The following trigPinechoPin cycle is used to determine the
	// distance of the nearest object by bouncing soundwaves off of it.  
	digitalWrite(trigPin, LOW); 
	delayMicroseconds(2); 

	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10); 

	digitalWrite(trigPin, LOW);
	duration = pulseIn(echoPin, HIGH);
 
        // Calculate the distance (in cm) based on the speed of sound.	
        LV.newival(duration / 58.2);

}

// =================================================
//
// humidity and Temperature Sensor Get Value
//
// =================================================

void HT_UpdateValue() {
      switch (DHT.read22(DHT22_PIN))
      {
        case DHTLIB_OK:  
    		//if (degublevel & HT_UpdateValue_DEBUG ) { Serial.print("HT OK,\t"); }
                AT.newival(DHT.temperature);
                AH.newival(DHT.humidity);                
    		break;
        case DHTLIB_ERROR_CHECKSUM: 
    		//if (degublevel & HT_UpdateValue_DEBUG ) { Serial.print("Checksum error,\t"); }
                AT.statusmsg("Checksum error");
                AH.statusmsg("Checksum error");
    		break;
        case DHTLIB_ERROR_TIMEOUT: 
    		//if (degublevel & HT_UpdateValue_DEBUG ) { Serial.print("Time out error,\t"); }
                AT.statusmsg("Time out error");
                AH.statusmsg("Time out error");
   		break;
        default: 
    		//if (degublevel & HT_UpdateValue_DEBUG ) { Serial.print("Unknown error,\t"); } 
                AT.statusmsg("Unknown error");
                AH.statusmsg("Unknown error");
    		break;
      }
      // Debug
   //   if (degublevel & HT_UpdateValue_DEBUG ) {
    //    Serial.print(DHT.humidity, 1);
    //    Serial.print(",\t");
    //    Serial.println(DHT.temperature, 1);
    //  }
}

// =================================================
//
// Serial Port 3 read - PH
//  Pins 14,15 
//
// =================================================
int PHbuflen = 0;
void serialEvent3(){                                                   //if the hardware serial port_3 receives a char 
  char inchar = (char)Serial3.read();                                //get the char we just received
  //if (!sensor_PHstringcomplete & PHbuflen < (PHSensorBuffLen-1)) {                                        // added to prevent buffer overrun
  if (PHbuflen < (PHSensorBuffLen-1)) {                                        // added to prevent buffer overrun
    PHsensorstring += inchar;                                          //add it to the inputString
    PHbuflen++;
  //Serial.println(inchar);
  }  
  if(inchar == '\r') {
    PH_UpdateValue();
    //sensor_PHstringcomplete = true;
  }               //if the incoming character is a <CR>, set the flag 
}

// =================================================
//
// PH Sensor Get Value
//
// =================================================
void PH_UpdateValue() {
  /*
  
   Ver 5.0 Firmware PH Probe  https://www.atlas-scientific.com/_files/_datasheets/_circuit/pH_Circuit_5.0.pdf
   Ver 5.?? PH Probe https://www.atlas-scientific.com/_files/_datasheets/_circuit/pH_EZO_datasheet.pdf?
   Responce Codes are: 
   
   *ER An unknown command has been sent 
   *OV The circuit is being ovearvolted (VCC>=5.5V)
   *UV The circuit is being undervolted (VCC<=3.1V)
   *RS The circuit has reset 
   *RE The circuit has completed boot up
   *SL The circuit has been put to sleep
   *WA The circuit has woken up from sleep
   
   */
//  if (sensor_PHstringcomplete){                                                   //if a string from the Atlas Scientific product has been received in its entierty 
//    String s1;
//    s1 = PHsensorstring;
//    sensor_PHstringcomplete = false;                                           //reset the flag used to tell if we have received a completed string from the Atlas Scientific product

    if (PHsensorstring == "*OK" ) {
      PH.statusmsg("Calibration Ok");
    }
    else if (PHsensorstring == "*ER" ) {
      PH.statusmsg("Unknown command");
    }  
    else if (PHsensorstring == "*OV" ) {
      PH.statusmsg("Under Volts");
    }  
    else if (PHsensorstring == "*UV" ) {
       PH.statusmsg("Over Volts");
   }  
    else if (PHsensorstring == "*RS" ) {
      PH.statusmsg("Reset");
    }  
    else if (PHsensorstring == "*RE" ) {
      PH.statusmsg("Ready");
    }  
    else if (PHsensorstring == "*SL" ) {
      PH.statusmsg("Sleep");
    }  
    else if (PHsensorstring == "*WA" ) {
      PH.statusmsg("Awake");
    }  
    else
    {
      char char1[PHsensorstring.length() +1];
      PHsensorstring.toCharArray(char1, PHsensorstring.length() +1);  
      PH.newival(atof(char1));
    }  
    PHsensorstring = "";                                                       //clear the string:
    PHbuflen = 0;                                                              // Reset buffer counter
}

// =================================================
//
// Serial Port 2 read - DO
//  Pins 16,17
//
// =================================================
int DObuflen = 0;
void serialEvent2(){                                                      //if the hardware serial port_3 receives a char 
  char inchar = (char)Serial2.read();                                     //get the char we just received
  //if (!sensor_DOstringcomplete & DObuflen < (DOSensorBuffLen-1)) {      // added to prevent buffer overrun
  if (DObuflen < (DOSensorBuffLen-1)) {                                   // added to prevent buffer overrun
    DOsensorstring += inchar;                                             //add it to the inputString
    DObuflen++;
  }  
  if(inchar == '\r') {
   // Serial.println("DO String");
   // Serial.println(DOsensorstring);

    char char1[DOsensorstring.length()+1];
    float ivalue;  
    DOsensorstring.toCharArray(char1,DOsensorstring.length());
    ivalue = atof(char1);
    DO.newival(ivalue);
    DO.statusmsg("Ok");
    DOsensorstring = "";                                                       //clear the string:
    DObuflen = 0;                                                              // Reset buffer counter

    //sensor_DOstringcomplete = true;
  }               //if the incoming character is a <CR>, set the flag 
}
