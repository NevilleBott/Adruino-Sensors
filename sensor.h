//
//
//
//

#include <Arduino.h>
#include <stdio.h>

#define maxstatsmsglen  40
#define maxnamelen  10


#ifndef __sensor
#define __sensor


class sensor {

public:
	
	sensor(char *name,float ifvalue,float ifconst,float minv,float maxv, boolean ienabled, boolean debug);
	float ivalue; 

        boolean filteredinrange();
	boolean newival(float newval);
        inline void Enable(boolean en) 
        { 
          enabled = en; 
          if (enabled) {
            strcpy(_statusmsg,"Enabled");   
          }
          else
          {
            strcpy(_statusmsg,"Disabled");   
          }  
        };
        inline void Debug(boolean debug)
        {
          _debugenabled = debug;
        };  
        
        inline void ToggleEnable() { 
          enabled = !enabled;         
          if (enabled) {
            strcpy(_statusmsg,"Enabled");   
          }
          else
          {
            strcpy(_statusmsg,"Disabled");   
          }        
        };
        
        boolean IsEnabled() { return enabled; };
        
        void statusmsg(char *msg) { 
            if ( strlen(msg) > 0 ) {
              strncpy(_statusmsg,msg,maxstatsmsglen); 
               if ( _debugenabled ) { 
                 Serial.println(_name);
                 Serial.println(_statusmsg);
              }
           }       
        };
        inline char *getstatusmsg(void) { return _statusmsg;} ;

        inline float gfvalue() { return fvalue; };
        inline char *ivalueStr() { 
          char s1[10];    
          dtostrf(ivalue, 5, 2, s1);
          return s1;
        };

        inline char *fvalueStr() {    
          char s1[10];    
          dtostrf(fvalue, 5, 2, s1);
          return s1;
        };

        inline void setfconst(float newval) { fconst = newval; };
        inline float getfconst() { return fconst; };
        inline char *getname() { return _name; };
        // to CSV
	inline char *toCSV() {
          char _buf[80];
          strcpy(_buf,_name);
          strcat(_buf,",");
          strcat(_buf,ivalueStr());
          strcat(_buf,",");
          strcat(_buf,fvalueStr());
          strcat(_buf,",");
          strcat(_buf,_statusmsg);          
          return _buf;
        };
        
        boolean calibrated;
        void command(String comstring);       
        
private:
        char _name[maxnamelen+1];
	boolean enabled;
	float fconst;
	float fvalue;
        // 16 = 6 + 6 
      //  char _strbuf[maxnamelen+1 + 18 + maxstatsmsglen+1];
        float _minv;
        float _maxv;
      //  char s1[6];
  	char _statusmsg[maxstatsmsglen+1];
protected:
        boolean _debugenabled;
};

#endif
