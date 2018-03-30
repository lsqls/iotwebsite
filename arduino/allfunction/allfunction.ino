#include "SIM900.h"
#include <SoftwareSerial.h>
//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
#include "sms.h"
#include "gps.h"
#include "inetGSM.h"
SMSGSM sms;
GPSGSM gps;
InetGSM inet;
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to send and receive SMS.

int numdata;
boolean started=false;
char smsbuffer[160];
char n[20];
char lon[15]="116.0";//set default lon,lat
char lat[15]="40.0";
char alt[15];
char time[20];
char vel[15];
char msg1[5];
char msg2[5];
char stat;
char inSerial[20];
int i=0;
char msg[50];
char datastr[50];
float value;
char* valuestr;
char buffer[4];
int sendSMS(char* number,char* message)
{
   //Enable this two lines if you want to send an SMS.
    if (sms.SendSMS(number, message))
      Serial.println("\nSMS sent OK");
      return 1;
    return 0;
}
int ggps()
{
        if (gps.attachGPS())
      Serial.println("gpsstatus=GPSREADY");
    else 
    {
      Serial.println("gpsstatus=ERROR");
      return 0;
    }
  
    delay(20000); //Time for fixing
    stat=gps.getStat();
  if(stat==1)
    Serial.println("NOT FIXED");
  else if(stat==0)
    Serial.println("GPS OFF");
  else if(stat==2)
    Serial.println("2D FIXED");
  else if(stat==3)
    Serial.println("3D FIXED");
  delay(5000);

  gps.getPar(lon,lat,alt,time,vel);
  Serial.println(lon);
  Serial.println(lat);
  Serial.println(alt);
  Serial.println(time);
  Serial.println(vel);
  return 1;

}
void httpg(char* host,int port,char* path)
{
  if (inet.attachGPRS("internet.wind", "", ""))
      Serial.println("gprsstatus=ATTACHED");
    else Serial.println("gprsstatus=ERROR");
    delay(1000);
    
    //Read IP address.
    gsm.SimpleWriteln("AT+CIFSR");
    delay(5000);
    //Read until serial buffer is empty.
    gsm.WhileSimpleRead();
  
    //TCP Client GET, send a GET request to the server and
    //save the reply.
   // numdata=inet.httpGET("www.baidu.com", 80, "/", msg, 50);//
    numdata=inet.httpGET(host, port, path, msg, 50);
    //Print the results.
    Serial.println("\nNumber of data received:");
    Serial.println(numdata);  
    Serial.println("\nData received:"); 
    Serial.println(msg); 
  
  }
float getvalue()
{  
  float value=500.0;
  return value;
}

void setup() 
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)){
    Serial.println("\nGSMstatus=READY");
    started=true;  
  }
  else Serial.println("\nstatus=IDLE");
  
  if(started)
  {
    sendSMS("13051680866","start");
    ggps();//lon lat
    httpg("iot.myworkroom.cn",80,"/");
    delay(1000);
  }
};

void loop() 
{
  value=getvalue();
  if(value>1000)  sendSMS("13051680866","fire");
   ggps();
   valuestr=dtostrf(value,3,2,buffer);
   sprintf(datastr, "/smoke/upload/%s-%s-%s", valuestr,lon,lat);
   Serial.println(datastr);
   httpg("iot.myworkroom.cn",80,datastr);
 //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  serialhwread();
  //Read for new byte on NewSoftSerial.
  serialswread();
  delay(3000);
};
 void serialhwread()
 {
  i=0;
  if (Serial.available() > 0){            
    while (Serial.available() > 0) {
      inSerial[i]=(Serial.read());
      delay(10);
      i++;      
    }
    
    inSerial[i]='\0';
    if(!strcmp(inSerial,"/END")){
      Serial.println("_");
      inSerial[0]=0x1a;
      inSerial[1]='\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if(!strcmp(inSerial,"TEST")){
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }
    //Read last message saved.
    if(!strcmp(inSerial,"MSG")){
      Serial.println(msg);
    }
    else{
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }    
    inSerial[0]='\0';
  }
}

void serialswread(){
  gsm.SimpleRead();
}
