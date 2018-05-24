#include <SoftwareSerial.h>
#include "SIM900.h"
#include "sms.h"
#include "inetGSM.h"
SoftwareSerial GpsSerial(5, 6);//定义一个软串口，5是RX，6是TX
struct//定义用于储存信息的结构体
{
  char GPS_Buffer[80];
  bool isGetData;    //是否获取到GPS数据
  bool isParseData; //是否解析完成
  char UTCTime[11];   //UTC时间
  char latitude[11];    //纬度
  char N_S[2];    //N/S
  char longitude[12];   //经度
  char E_W[2];    //E/W
  bool isUsefull;   //定位信息是否有效
} Save_Data;
SMSGSM sms;
InetGSM inet;
int smokepin = A0;
int numdata;
boolean started = false;
int i = 0;
char msg[50];
char datastr[30];
float value;
char* valuestr;
char buffer[4];
int L = 13; //LED指示灯引脚
const unsigned int gpsRxBufferLength = 300;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;
void setup()
{
  Serial.begin(9600);
  GpsSerial.begin(9600);//定义波特率9600，GPS模块输出的波特率一致
  Serial.println(F("Wating..."));
  Save_Data.isGetData = false;
  Save_Data.isParseData = false;
  Save_Data.isUsefull = false;
  Serial.println(F("GSM.."));
  if (gsm.begin(9600)) {
    Serial.println(F("READY"));
    started = true;
  }
  else Serial.println(F("IDLE"));
  pinMode(smokepin, INPUT);
  if (started)
  {
    delay(3000);
    sendSMS("13051680866", "start");
    httpg("iot.myworkroom.cn", 80, "/");
   // ggps();//lon lat
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
  Serial.print(F("DataString:"));
  Serial.println(datastr);
  httpg("iot.myworkroom.cn", 80, datastr);
  gsm.SimpleRead();
  delay(3000);
};


void sendSMS(char* number, char* message)
{
  if (sms.SendSMS(number, message))
    Serial.println(F("message send success"));
}

void ggps()
{
  gpsRead();  
  parseGpsBuffer();
}
void httpg(char* host, int port, char* path)
{
  if (inet.attachGPRS("cmnet", "", ""))//原本填的是internet.wind,3gnet是联通的接入点
    Serial.println(F("gprs attached"));
  else Serial.println(F("gprs eorro"));
  delay(1000);
//  gsm.SimpleWriteln("AT+CIFSR");
//  delay(5000);
//  gsm.WhileSimpleRead();
  numdata = inet.httpGET(host, port, path, msg, 50);
  if(numdata)
    Serial.println(F("http get request success"));
}
float getvalue()
{
  float value = analogRead(smokepin);
  return value;
}
void errorLog(int num)//错误输出
{
  Serial.print(F("ERROR"));
  Serial.println(num);
  while (1)
  {
    digitalWrite(L, HIGH);
    delay(300);
    digitalWrite(L, LOW);
    delay(300);
  }
}
void printGpsBuffer()//打印gps信息 
{  
  if (!Save_Data.isParseData)
         Serial.println(F("GPS DATA is not usefull!"));//当无法接收到GPS信息时，打印“GPS DATA is not usefull!”

}
/* parseGpsBuffer函数用于解析GPS模块发送的原始数据$GPRMC,094606.00,A,4009.42840,N,11616.92903,E,0.395,,070418,,,A*7F这是GPS的原始数据，我们需要对这串字符串进行处理，基本思路是使用strstr函数查找“，”，将“，”作为分割符，分割字符串，获得各项信息。
strstr函数说明：strstr(str1,str2)  str1: 被查找目标.str2: 要查找对象返回值：若str2是str1的子串，则返回str2在str1的首次出现的地址；如果str2不是str1的子串，则返回NULL。*/
void parseGpsBuffer()
{
  char *subString;
  char *subStringNext;
  if (Save_Data.isGetData)
  {
    Save_Data.isGetData = false;
    for (int i = 0 ; i <= 6 ; i++)
    {
      if (i == 0)
      {
        if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
          errorLog(1);  //解析错误
      }
      else
      {
        subString++;
        if ((subStringNext = strstr(subString, ",")) != NULL)
        {
          char usefullBuffer[2];
          switch (i)
          {
            case 1: memcpy(Save_Data.UTCTime, subString, subStringNext - subString); break; //获取UTC时间
            case 2: memcpy(usefullBuffer, subString, subStringNext - subString); break; //获取UTC时间
            case 3: memcpy(Save_Data.latitude, subString, subStringNext - subString); break; //获取纬度信息
            case 4: memcpy(Save_Data.N_S, subString, subStringNext - subString); break; //获取N/S
            case 5: memcpy(Save_Data.longitude, subString, subStringNext - subString); break; //获取纬度信息
            case 6: memcpy(Save_Data.E_W, subString, subStringNext - subString); break; //获取E/W
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
          errorLog(2);  //解析错误
        }
      }
    }
  }
}
void clrGpsRxBuffer(void)//清空GPS串口缓冲区信息
{
  memset(gpsRxBuffer, 0, gpsRxBufferLength);      //清空
  ii = 0;
}
void gpsRead() //读取GPS串口的信息
{
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





