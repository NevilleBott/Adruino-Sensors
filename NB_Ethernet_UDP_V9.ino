/*
  Arduino Fish Tank

 */

#include <SPI.h>
#include <Ethernet.h>
#include <EthernetUdp.h>
#include <OneWire.h>
#include <SD.h>
#include <dht.h>
//#include <Time.h>

#include "sensor.h"
#include "FlowMeter.h"
#include "DSTemp.h"

#include "device.h"

// =================================================
// Temp  Humidity
// =================================================
#define DHT22_PIN 6 // This was working on pin 6
dht DHT;

// =================================================
// Level HC SR004
//
//
// This uses serial port pins not sure if this is necessary - Test with another address
//
// =================================================
#define echoPin 19  //Echo Pin
#define trigPin 18  //Trigger Pin
//#define echoPin 9  //Echo Pin
//#define trigPin 8  //Trigger Pin

int maximumRange = 200;  //Maximum range needed
int minimumRange = 0;  //Minimum range needed
long duration, distance;  //Duration used to calculate distance

// =================================================
//   One Wire DS18S20
// =================================================
int DS18S20_Pin1 = 2; //DS18S20 Signal pin on digital 2
int DS18S20_Pin2 = 3; //DS18S20 Signal pin on digital 2
// Temp Probe 1 0x28, 0x93, 0x3E, 0xE3, 0x05, 0x00, 0x00, 0x24
// Temp Probe 2 0x28, 0xD1, 0x5D, 0xEB, 0x05, 0x00, 0x00, 0x7C

//Temperature chip I/O
//OneWire ds1(DS18S20_Pin1);  // on digital pin 2
//OneWire ds2(DS18S20_Pin2);  // on digital pin 2

// Flow meter
byte sensorInterrupt = 2;
byte sensorPin  = 21;


// =================================================
//
//  SD Card
//
// http://forum.freetronics.com/viewtopic.php?t=682
//
// =================================================

// On the Ethernet Shield, CS is pin 4. Note that even if it's not
// used as the CS pin, the hardware CS pin (10 on most Arduino boards,
// 53 on the Mega) must be left as an output or the SD library
// functions will not work.
const int chipSelect = 53;
const int SDcardPin = 4;

boolean logenabled;
boolean Webenabled;
boolean UDPenabled;
boolean UDPSendEnabled;

// sd file name
//File myFile;

// set up variables using the SD utility library functions:
//Sd2Card card;
//SdVolume volume;
//SdFile root;

// =================================================
// RTC
// http://forum.freetronics.com/viewtopic.php?t=682
//
// =================================================

// =================================================
//
//  System State Map Variables
//
// =================================================
static int SystemState = 0;
static int NextSystemState = 0;
unsigned long steptime = 0;
unsigned long prevtime = 0;


// =================================================
//  Set Debug Level
// =================================================
int degublevel = 0;
#define loop_DEBUG 1
#define loop_DEBUG1 2
#define loop_DEBUG2 4
#define loop_DEBUG3 8
#define loop_DEBUG4 16
#define loop_DEBUG5 32
#define loop_DEBUG6 64
#define loop_DEBUG7 128
#define Web_DEBUG 16384
#define UDP_DEBUG 32768

// =================================================
//
//  Sensor Data structures
//
// =================================================

DSTemp TT1("TT1", 20, 0.05, 0, 50, false, false, DS18S20_Pin1); // Tank Temperature
DSTemp TT2("TT2", 20, 0.05, 0, 50, false, false, DS18S20_Pin2); // Temperature 2
sensor PH("PH", 7, 0.01, 0, 14, false, false);
sensor DO("DO", 8.8, 0.01, 0, 20, false, false);
sensor AT("AT", 20, 0.01, 0, 50, false, false); // Air Temperature
sensor AH("AH", 60, 0.01, 0, 100, false, false); // Humidity
sensor LV("LV", 10, 0.1, 0, 100, false, false); // Level
FlowMeter FL("FL", 0, 0.1, 0, 60, true, false, sensorPin, sensorInterrupt); // Flow

// =================================================
// Pump and Airator
// =================================================
#define Pump1Pin 33   //Pump Pin
#define Pump2Pin 31   //Pump Pin
#define AirPin 34     //Air

device AIR("AIR", 15 * 60, 45 * 60, AirPin, false, false);  // Airator
device PMP1("PMP1", 15 * 60, 15 * 60, Pump1Pin, false, false); // Pump
device PMP2("PMP2", 15 * 60, 15 * 60, Pump2Pin, false, false); // Pump 2

float caltemp = 0.0;

const int PHSensorBuffLen = 30;
const int DOSensorBuffLen = 30;
const int DebugBuffLen = 30;

String inputstring = "";                                                       //a string to hold incoming data from the PC
String PHsensorstring = "";                                                      //a string to hold the data from the Atlas Scientific product
String DOsensorstring = "";                                                      //a string to hold the data from the Atlas Scientific product

// =================================================
//
//  Ethernet
//
// assign a MAC address for the ethernet controller.
// fill in your address here:
// =================================================
byte mac[] = {0xDE, 0xAD, 0xBE, 0x8D, 0x10, 0x2F};
// assign an IP address for the controller:
IPAddress ip(192, 168, 1, 21);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

//
// Udp Interface
//
byte UDP_Remoteip[4]; //(192,168,1,2);
unsigned int localUDPPort = 69;      // local port to listen on
unsigned int remoteUPDPort = 2180;      // remote port to send to
EthernetUDP Udp;
char packetBuffer[UDP_TX_PACKET_MAX_SIZE]; //buffer to hold incoming packet

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):

//EthernetServer server(80);

// Calibrate state
int callevel = 0;
int caldolevel = 0;
// =================================================
//
// Setup
// Ethernet
// Serial Port 1 - Debug
// Serial Port 2 DO Probe
// Serial Port 3 PH Probe
//
// =================================================
void setup() {

  //Serial.print("Reboot Begin");
  delay(500);

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);

  UDP_Remoteip[0] = 192;
  UDP_Remoteip[1] = 168;
  UDP_Remoteip[2] = 1;
  UDP_Remoteip[3] = 2;
  //UDP_Remoteip[3] = 30;

  Udp.begin(localUDPPort);
  //server.begin();

  Serial.begin(115200);                                                      //set baud rate for the hardware serial port_0 to 38400
  Serial2.begin(38400);                                                     //set baud rate for software serial port_3 to 38400
  Serial3.begin(38400);                                                     //set baud rate for software serial port_3 to 38400
  inputstring.reserve(DebugBuffLen);                                        //set aside some bytes for receiving data from the PC
  DOsensorstring.reserve(PHSensorBuffLen);                                    //set aside some bytes for receiving data from Atlas Scientific product
  PHsensorstring.reserve(PHSensorBuffLen);                                    //set aside some bytes for receiving data from Atlas Scientific product

  //  Serial.println(Ethernet.localIP());

  // give the sensor and Ethernet shield time to set up:
  delay(1000);

  Serial3.print('E');                                      // Single Read Mode
  Serial3.print('\r');

  Serial2.print('E');                                      // Single Read Mode DO single mode ???
  Serial2.print('\r');

  // Level Setup
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);


  Serial.println("Initializing SD card...");
  // Initialize sd card
  pinMode(chipSelect, OUTPUT); // set the SS pin as an output (necessary!) pin 53 on mega
  digitalWrite(chipSelect, HIGH); // but turn off the W5100 chip!
  delay(500);

  if (!SD.begin(SDcardPin)) {
    Serial.println("initialization failed!");
    //return;
    logenabled = false;
  }
  else {
    Serial.println("initialization done.");
    logenabled = false;
  }


  callevel = 0;
  PH.calibrated = false;
  TT1.Enable(true);
  TT2.Enable(true);

  DO.Enable(true);
  PH.Enable(true);

  AH.Enable(true);
  AT.Enable(true);
  LV.Enable(true);
  Webenabled = false;
  UDPenabled = true;
  UDPSendEnabled = true;

  AIR.Enable(true);
  PMP1.Enable(true);
  PMP2.Enable(true);

  AIR.Control(false);
  PMP1.Control(true);
  PMP2.Control(true);

  Serial.println("Ready");

  degublevel = 0; //Web_DEBUG + loop_DEBUG + loop_DEBUG1;

  // Enable the interrupt again now that we've finished sending output
  attachInterrupt(FL.sensorInterrupt, pulseCounter, FALLING);

}

/*
Insterrupt Service Routine Flow Meter
*/
void pulseCounter()
{
  // Increment the pulse counter
  FL.incpulseCount();
}

// =================================================
//
// Main Loop
//
// =================================================
void loop() {

  static int st = -1, prevst = 0;

  switch (SystemState) {

    case 0:                  // Check Times
      //steptime = millis() - prevtime;   // ######################################### Handle wrap around of millis ??
      // Serial.println("loop 0");
      if ((millis() - prevtime) > 100) {  // 100ms Second
        prevtime = millis();
        st += 1;
        if (degublevel & loop_DEBUG ) {
          Serial.print("Second No ");
          Serial.println(st);
        }

        //        Serial.println("Step ");
        //        Serial.println(steptime);
        //        Serial.println("Prev ");
        //        Serial.println(prevtime);
        switch (st) {
          case 0:                  // Read Temp
            if ( TT1.IsEnabled() ) {
              NextSystemState = 1;
            }
            break;
          case 1:                  // Spare
            break;
          case 2:                  // Read PH
            if ( PH.IsEnabled() ) {
              NextSystemState = 3;
            }
            break;
          case 3:                  // TT2
            if ( TT2.IsEnabled() ) {
              NextSystemState = 8;
            }
            break;
          case 4:                  // Read DO
            //Serial.print("S4");
            if ( DO.IsEnabled() ) {
              NextSystemState = 4;
            }
            break;
          case 5:                  // Level
            //Serial.print("S5");
            if ( LV.IsEnabled() ) {
              NextSystemState = 6;
            }
            break;
          case 6:                  // Log Data now UDP send
            //Serial.print("S6");
            if ( UDPSendEnabled )  {
              NextSystemState = 7;
            }
            break;
          case 7:                  // Read Temp & Humidity
            //Serial.print("S7");
            if ( AH.IsEnabled() ) {
              NextSystemState = 5;
            }
            break;

          case 8:

            // Disable the interrupt while calculating flow rate and sending the value to
            // the host
            detachInterrupt(sensorInterrupt);
            FL.ReadFlow();
            // Enable the interrupt again now that we've finished sending output
            attachInterrupt(sensorInterrupt, pulseCounter, FALLING);

            break;
          case 9:
            //Serial.print("S9");

            // Restart the Ethernet connection and the server:
            //            Ethernet.begin(mac, ip);
            //            Udp.begin(localPort);
            //            server.begin();
            break;

          default:                 // Reset Second
            st = -1;
            break;
        }
      }
      //delay(5);
      break;

    case 1:                  // Read Temp and Check if Cal Required

      TT1.getTemp();
      if (TT1.filteredinrange() & PH.IsEnabled() )  {
        if ( ((TT1.gfvalue() - caltemp) > 1.0) | ((TT1.gfvalue() - caltemp) <  -1.0) )  {
          //calrequired = true;
          NextSystemState = 2;
          if (degublevel & loop_DEBUG1 ) {
            Serial.println("Temp ");
            Serial.println(TT1.ivalue );
            Serial.println("Filtered Temp ");
            Serial.println(TT1.gfvalue());
            Serial.println("Cal Temp ");
            Serial.println(caltemp);
          }
        }
        else  {
          NextSystemState = 0;
        }
      }
      else {
        NextSystemState = 0;
      }

      break;
    case 2:                  // Send Calibration Temperature
      if (degublevel & loop_DEBUG2 ) {
        Serial.println("State 2 Calibrate");
        Serial.println("Calibration Temp = ");
        Serial.println(TT1.gfvalue());
      }
      PH.statusmsg("Calibrating");
      PH.calibrated = true;
      Serial3.print(TT1.gfvalue());
      Serial3.print('\r');
      caltemp = TT1.gfvalue();    // Save the new calibrated temp
      NextSystemState = 0;
      break;
    case 3:                  // Send Read PH Request
      //Serial.println("Cal Level");
      //Serial.println(callevel);
      switch (callevel) {
        case 0:
          if (degublevel  & loop_DEBUG3 ) {
            Serial.println("State 3 PH Read");
          }
          PH.statusmsg("Reading");
          Serial3.print("R");
          break;
        case 4:
          if (degublevel  & loop_DEBUG3 ) {
            Serial.println("State 3 Cal 4");
          }
          PH.statusmsg("Calibrate 4");
          Serial3.print("F");
          break;
        case 7:
          if (degublevel  & loop_DEBUG3 ) {
            Serial.println("State 3 Cal 7");
          }
          PH.statusmsg("Calibrate 7");
          Serial3.print("S");
          break;
        case 10:
          if (degublevel  & loop_DEBUG3 ) {
            Serial.println("State 3 Cal 10");
          }
          PH.statusmsg("Calibrate 10");
          Serial3.print("T");
          break;
        case 1:
          if (degublevel  & loop_DEBUG3 ) {
            Serial.println("State 3 Cal End");
          }
          PH.statusmsg("Calibrate End");
          Serial3.print("E");
          callevel = 0;
          break;
      }
      Serial3.print('\r');
      NextSystemState = 0;
      break;
    case 4:                  // Send Read DO Request

      //
      // Temp Cal T,19.5 then 300 ms before reading
      // Read then wait 1 s ??
      //
      //
      switch (caldolevel)
      {
        case 0:
          if (degublevel  & loop_DEBUG4 ) {
            Serial.println("State 4 DO Read");
          }
          DO.statusmsg("Reading");
          Serial2.print("R");        // ##################### Change to TT.TT
          break;

        case 1:      // Set Atmospehric Cal
          break;
        case 2:      // Clear Calibration
          break;
        case 3:      // Query Calibration
          break;
        case 4:      // Query done don't clear status
          break;
      }

      Serial2.print('\r');
      NextSystemState = 0;
      break;

    case 5:		// Humidity and Temprature
      HT_UpdateValue();
      NextSystemState = 0;
      break;

    case 6:		// Level
      Level_UpdateValue();
      NextSystemState = 0;
      break;

    case 7:

      char s1[500];

      strcpy(s1, TT1.toCSV());
      strcat(s1, ",");
      strcat(s1, TT2.toCSV());
      strcat(s1, ",");
      strcat(s1, PH.toCSV());
      strcat(s1, ",");
      strcat(s1, DO.toCSV());
      strcat(s1, ",");
      strcat(s1, AT.toCSV());
      strcat(s1, ",");
      strcat(s1, AH.toCSV());
      strcat(s1, ",");
      strcat(s1, LV.toCSV());
      strcat(s1, ",");
      strcat(s1, FL.toCSV());
      strcat(s1, ",");
      strcat(s1, AIR.toCSV());
      strcat(s1, ",");
      strcat(s1, PMP1.toCSV());
      strcat(s1, ",");
      strcat(s1, PMP2.toCSV());

      if (degublevel  & UDP_DEBUG ) {

        Serial.println("UDP Sending");
        Serial.println(s1);
      }

      Udp.beginPacket(UDP_Remoteip, remoteUPDPort);
      Udp.write(s1);
      Udp.endPacket();


      NextSystemState = 0;
      break;

    case 8:
      TT2.getTemp();
      NextSystemState = 0;
      break;

    default:
      NextSystemState = 0;
      break;
  }
  //
  // State machine update next state
  //
  if (NextSystemState != SystemState) {
    if (degublevel  & loop_DEBUG ) {
      Serial.print("State Change From -> ");
      Serial.print(SystemState);
      Serial.print(" to ");
      Serial.print(NextSystemState);
      Serial.print('\r');
      Serial.print('\n');
    }
    SystemState = NextSystemState;
  }

  // listen for incoming Ethernet connections:
  // if ( Webenabled ) { listenForEthernetClients(); };

  int packetSize = Udp.parsePacket();
  if (packetSize > 1 ) {
    int ch = 0;
    while (ch < UDP_TX_PACKET_MAX_SIZE - 2 & ch < packetSize) {
      packetBuffer[ch] = Udp.read();
      ch += 1;
    }
    packetBuffer[ch] = 0;

    Serial.print("Received UDP Packet ");
    // Serial.println(packetBuffer);
    if (UDPenabled ) {
      Serial.println("Parsing UDP");
      Serial.println("packetBuffer");

      Serial.println("Parsing UDP Length");
      Serial.println(packetSize);
      ParseSerialCommmand(packetBuffer);
    } else {
      Serial.println("Recieved but UDP Disabled");
    }
  }

  if (PMP1.CheckTime()) {
    FL.ResetTotal();
  }
  PMP2.CheckTime();
  AIR.CheckTime();

}  // End Main Loop

