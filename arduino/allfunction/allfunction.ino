#include "SIM900.h"
#include <SoftwareSerial.h>

//#define GpsSerial  Serial
#define DebugSerial Serial
int L = 13; 
SoftwareSerial GpsSerial(5, 6);
struct
{
  char GPS_Buffer[80];
  bool isGetData;    
  bool isParseData; 
  char UTCTime[11];  
  char latitude[11];   
  char N_S[2];   
  char longitude[12];   
  char E_W[2];    
  bool isUsefull; 
} Save_Data;



//If not used, is better to exclude the HTTP library,
//for RAM saving.
//If your sketch reboots itself proprably you have finished,
//your memory available.
//#include "inetGSM.h"

//If you want to use the Arduino functions to manage SMS, uncomment the lines below.
#include "sms.h"
//#include "gps.h"
#include "inetGSM.h"
SMSGSM sms;
//GPSGSM gps;
InetGSM inet;
int smokepin = A0;
//To change pins for Software Serial, use the two lines in GSM.cpp.

//GSM Shield for Arduino
//www.open-electronics.org
//this code is based on the example of Arduino Labs.

//Simple sketch to send and receive SMS.
const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;
int numdata;
boolean started = false;
//char smsbuffer[160];
char n[20];
//char msg1[5];
//char msg2[5];
//char stat;
char inSerial[20];
int i = 0;
char msg[50];
char datastr[30];
float value;
char* valuestr;
char buffer[4];
int sendSMS(char* number, char* message)
{
  //Enable this two lines if you want to send an SMS.
  if (sms.SendSMS(number, message))
    Serial.println("\nSMS sent OK");
  return 1;
  return 0;
}

void ggps()
{
  gpsRead();  
  parseGpsBuffer();
  //printGpsBuffer();
}
void httpg(char* host, int port, char* path)
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
  numdata = inet.httpGET(host, port, path, msg, 50);
  //Print the results.
  Serial.println("\nNumber of data received:");
  Serial.println(numdata);
  Serial.println("\nData received:");
  Serial.println(msg);

}
float getvalue()
{
  float value = analogRead(smokepin);
  Serial.print("Value:");
  Serial.println(value);
  return value;
}

void setup()
{
  //Serial connection.
  Serial.begin(9600);
  Serial.println("GSM Shield testing.");
  //Start configuration of shield with baudrate.
  //For http uses is raccomanded to use 4800 or slower.
  if (gsm.begin(2400)) {
    Serial.println("\nGSMstatus=READY");
    started = true;
  }
  else Serial.println("\nstatus=IDLE");
  pinMode(smokepin, INPUT);
  if (started)
  {
    sendSMS("13051680866", "start");
    httpg("iot.myworkroom.cn", 80, "/");
    ggps();//lon lat
    delay(1000);
  }
};

void loop()
{
  value = getvalue();
  if (value > 1000)  sendSMS("13051680866", "fire");
  ggps();
  valuestr = dtostrf(value, 3, 2, buffer);
  if (Save_Data.isUsefull)
    sprintf(datastr, "/smoke/upload/%s-%s-%s", valuestr, Save_Data.longitude, Save_Data.latitude);
  else
    sprintf(datastr, "/smoke/upload/%s", valuestr);
  Serial.print("DataString:");
  Serial.println(datastr);
  httpg("iot.myworkroom.cn", 80, datastr);
  //Read for new byte on serial hardware,
  //and write them on NewSoftSerial.
  serialhwread();
  //Read for new byte on NewSoftSerial.
  serialswread();
  delay(3000);
};
void serialhwread()
{
  i = 0;
  if (Serial.available() > 0) {
    while (Serial.available() > 0) {
      inSerial[i] = (Serial.read());
      delay(10);
      i++;
    }

    inSerial[i] = '\0';
    if (!strcmp(inSerial, "/END")) {
      Serial.println("_");
      inSerial[0] = 0x1a;
      inSerial[1] = '\0';
      gsm.SimpleWriteln(inSerial);
    }
    //Send a saved AT command using serial port.
    if (!strcmp(inSerial, "TEST")) {
      Serial.println("SIGNAL QUALITY");
      gsm.SimpleWriteln("AT+CSQ");
    }
    //Read last message saved.
    if (!strcmp(inSerial, "MSG")) {
      Serial.println(msg);
    }
    else {
      Serial.println(inSerial);
      gsm.SimpleWriteln(inSerial);
    }
    inSerial[0] = '\0';
  }
}

void serialswread() {
  gsm.SimpleRead();
}
void errorLog(int num)
{
  DebugSerial.print("ERROR");
  DebugSerial.println(num);
  while (1)
  {
    digitalWrite(L, HIGH);
    delay(300);
    digitalWrite(L, LOW);
    delay(300);
  }
}

void printGpsBuffer()
{
  if (Save_Data.isParseData)
  {
    Save_Data.isParseData = false;

    DebugSerial.print("Save_Data.UTCTime = ");
    DebugSerial.println(Save_Data.UTCTime);

    if (Save_Data.isUsefull)
    {
      Save_Data.isUsefull = false;
      DebugSerial.print("Save_Data.latitude = ");
      DebugSerial.println(Save_Data.latitude);
      DebugSerial.print("Save_Data.N_S = ");
      DebugSerial.println(Save_Data.N_S);
      DebugSerial.print("Save_Data.longitude = ");
      DebugSerial.println(Save_Data.longitude);
      DebugSerial.print("Save_Data.E_W = ");
      DebugSerial.println(Save_Data.E_W);
    }
    else
    {
      DebugSerial.println("GPS DATA is not usefull!");
    }

  }
}

void parseGpsBuffer()
{
  char *subString;
  char *subStringNext;
  if (Save_Data.isGetData)
  {
    Save_Data.isGetData = false;
    DebugSerial.println("**************");
    DebugSerial.println(Save_Data.GPS_Buffer);


    for (int i = 0 ; i <= 6 ; i++)
    {
      if (i == 0)
      {
        if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
          errorLog(1); 
      }
      else
      {
        subString++;
        if ((subStringNext = strstr(subString, ",")) != NULL)
        {
          char usefullBuffer[2];
          switch (i)
          {
            case 1: memcpy(Save_Data.UTCTime, subString, subStringNext - subString); break; 
            case 2: memcpy(usefullBuffer, subString, subStringNext - subString); break; 
            case 3: memcpy(Save_Data.latitude, subString, subStringNext - subString); break; 
            case 4: memcpy(Save_Data.N_S, subString, subStringNext - subString); break; 
            case 5: memcpy(Save_Data.longitude, subString, subStringNext - subString); break; 
            case 6: memcpy(Save_Data.E_W, subString, subStringNext - subString); break;

            default: break;
          }

          subString = subStringNext;
          Save_Data.isParseData = true;
          if (usefullBuffer[0] == 'A')
            Save_Data.isUsefull = true;
          else if (usefullBuffer[0] == 'V')
            Save_Data.isUsefull = false;

        }
        else
        {
          errorLog(2);  
        }
      }


    }
  }
}


void clrGpsRxBuffer(void)
{
  memset(gpsRxBuffer, 0, gpsRxBufferLength);      
  ii = 0;
}
void gpsRead() {
  while (GpsSerial.available())
  {
    gpsRxBuffer[ii++] = GpsSerial.read();
    if (ii == gpsRxBufferLength)clrGpsRxBuffer();
  }

  char* GPS_BufferHead;
  char* GPS_BufferTail;
  if ((GPS_BufferHead = strstr(gpsRxBuffer, "$GPRMC,")) != NULL || (GPS_BufferHead = strstr(gpsRxBuffer, "$GNRMC,")) != NULL )
  {
    if (((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL) && (GPS_BufferTail > GPS_BufferHead))
    {
      memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);
      Save_Data.isGetData = true;

      clrGpsRxBuffer();
    }
  }
}





