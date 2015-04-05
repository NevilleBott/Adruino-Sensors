//
//
//
//

//#define boolean bool
#include <Arduino.h>
//#include <stdio.h>

#define maxstatsmsglen  40
#define maxnamelen  10

class device {

public:
	
	device(char *name,unsigned long ontime,unsigned long offtime,int pinno, boolean enabled,boolean stat);
        //
        // Enable functions
        //
        inline void Enable(boolean enable) 
        { 
          _enabled = enable; 
          if (_enabled) {
            strcpy(_statusmsg,"Enabled");   
          }
          else
          {
            strcpy(_statusmsg,"Disabled");   
          }  
        };
        inline void ToggleEnable() { 
          _enabled = !_enabled;         
          if (_enabled) {
            strcpy(_statusmsg,"Enabled");   
          }
          else
          {
            strcpy(_statusmsg,"Disabled");   
          }        
        };
        boolean IsEnabled() { return _enabled; };

        inline void Debug(boolean debug)
        {
          _debugenabled = debug;
        };  

        //
        // change Time functions
        //
        inline void OnTime(int ontime)    { _ontime = (unsigned long)ontime;   } ;
        inline void OffTime(int offtime)  { _offtime = (unsigned long)offtime; };
        //
        // Status Message functions
        //
        inline void statusmsg(char *msg) { strcpy(_statusmsg,msg); };
        inline char *getstatusmsg(void) { return _statusmsg;} ;
 
        //
        // Control function
        //
        void Control(boolean __on);

        boolean CheckTime();
        char *toXMLStringTime();
        char *toCSV();  
        char *TimeToGo(); 
       
        void command(String comstring);       
        
private:
	boolean _enabled;
	boolean _on;
        int _Pin;
        char _name[maxnamelen+1];
  	char _statusmsg[maxstatsmsglen+1];
        char _strbuf[maxnamelen+1 + 14 + maxstatsmsglen+1];
        
        unsigned long _starttime;
        unsigned long _stoptime;
        unsigned long _ontime;
        unsigned long _offtime;
        char chrs[8];  

protected:
        boolean _debugenabled;
};

