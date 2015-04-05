

// =================================================
//
// Serial Port 1 (Debug) read
//
//
// =================================================
void serialEvent() {                                                         //if the hardware serial port_0 receives a char              
  char inchar = (char)Serial.read();                               //get the char we just received
  inputstring += inchar;                                           //add it to the inputString
  if(inchar == '\r') {            
    Serial.print("New Char in from Serial =");
    Serial.println(inputstring);
    ParseSerialCommmand(inputstring);
    inputstring = "";                                                        //clear the string:
 
  }                //if the incoming character is a <CR>, set the flag
}  

// =================================================
//
// Process Input Commands from Serial
//
// =================================================
void ParseSerialCommmand(String instring) {
  // Commands  
    Serial.print("Parsing Command ");
    Serial.println(instring);
    int iNewVal = 0;
    boolean ibool;
    float fNewVal;
    
    if (instring.startsWith("?") ) {
      Serial.println("DEBUGn = Debug Level");
      Serial.println("K(T,P,D) = Filter constants");            
      Serial.println("TT1 = Tank Temp 1");
      Serial.println("TT2 = Tank Temp 2");
      Serial.println("DO = DO");
      Serial.println("PH = PH");
      Serial.println("AH = Humidity");
      Serial.println("AT = Air Temp");
      Serial.println("LV = Tank Level");
      Serial.println("FL = Flow");

      Serial.println("WEB = WEB");
      Serial.println("UDP = UDP");
      Serial.println("UDP Port = UDPPort");
      
      Serial.println("WEB = Web Toggle");
      Serial.println("UDP = UDP Toggle");

      Serial.println("IPa,b,c,d = IP Address for UDP Send");
      Serial.println("CAL0,7,4,10 For PH Probe");

    }

    else if (instring.startsWith("DEBUG")) {      
        Serial.println("Set Debug From");
        Serial.println(degublevel);
        Serial.println("Set Debug To");
        iNewVal = instring.substring(5).toInt();   
        Serial.println(iNewVal);
        degublevel = iNewVal;
    }  
    
    else if (instring.startsWith("TT1")) { TT1.command(instring.substring(3)); }
    else if (instring.startsWith("TT2")) { TT2.command(instring.substring(3)); }

    else if (instring.startsWith("PH")) { PH.command(instring.substring(2)); }
    else if (instring.startsWith("DO")) { DO.command(instring.substring(2)); }
    else if (instring.startsWith("AT")) { AT.command(instring.substring(2)); }
    else if (instring.startsWith("AH")) { AH.command(instring.substring(2)); }
    else if (instring.startsWith("LV")) { LV.command(instring.substring(2)); }
    else if (instring.startsWith("FL")) { FL.command(instring.substring(2)); }

    else if (instring.startsWith("P1")) { PMP1.command(instring.substring(2)); }
    else if (instring.startsWith("P2")) { PMP2.command(instring.substring(2)); }
    else if (instring.startsWith("A1")) { AIR.command(instring.substring(2));  }
       
    else if (instring.startsWith("WEB")) {
         Webenabled = !Webenabled;
         if (Webenabled ) {
             Serial.println("Web Enabled");
         } else {
             Serial.println("Web Disabled");
         }          
     }  
     else if (instring.startsWith("UDPPort")) {
         remoteUPDPort = instring.substring(7).toInt();
         Serial.println("Remote Port");
         Serial.println(remoteUPDPort);
         UDPSendEnabled = true;
     }
     else if (instring.startsWith("UDP")) {
         UDPSendEnabled = !UDPSendEnabled;
         if (UDPSendEnabled ) {
             Serial.println("UDP Enabled");
         } else {
             Serial.println("UDP Disabled");
         }          
     }

     else if (instring.startsWith("CAL0")) {
         callevel = 1;
         Serial.println(callevel);
     }
     else if (instring.startsWith("CAL7")) {
         callevel = 7;
         Serial.println(callevel);
     }
     else if (instring.startsWith("CAL4")) {
         callevel = 4;
         Serial.println(callevel);
     }
     else if (instring.startsWith("CAL10")) {
         callevel = 10;
         Serial.println(callevel);
     }

     else if (instring.startsWith("CALDO")) {
         caldolevel = 1;
         Serial.println(callevel);
     }


     // eg 192.168.1.23
     else if (instring.startsWith("IPa")) {
          String sx = instring.substring(3,7);
          Serial.print("New IP String :");
          Serial.println(sx);          
          UDP_Remoteip[0] = ParseIP(sx);
     }
     else if (instring.startsWith("IPb")) {
          String sx = instring.substring(3,7);
          Serial.print("New IP String :");
          Serial.println(sx);          
          UDP_Remoteip[1] = ParseIP(sx);
     }
     else if (instring.startsWith("IPc")) {
          String sx = instring.substring(3,7);
          Serial.print("New IP String :");
          Serial.println(sx);          
          UDP_Remoteip[2] = ParseIP(sx);
     }
     else if (instring.startsWith("IPd")) {
          String sx = instring.substring(3,7);
          Serial.print("New IP String :");
          Serial.println(sx);          
          UDP_Remoteip[3] = ParseIP(sx);
     }

}

byte ParseIP(String s)
{
    char s1[4]; 
    s.toCharArray(s1,4);
    int newval = atoi(s1);
    Serial.print("IP :");
    Serial.println(newval);
    if (newval > 10 & newval < 254) {
        return byte(newval);
    }  
}  
