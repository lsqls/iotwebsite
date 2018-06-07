#include <SoftwareSerial.h>
#include <OneWire.h> 
#include <DallasTemperature.h>
#include "SIM900.h"//控制sim800模块的库文件
#include "sms.h"//控制sim800模块的库文件
#include "inetGSM.h"//控制sim800模块的库文件
//SoftwareSerial GpsSerial(5, 6);//定义一个软串口，5是RX，6是TX，用于接收gps数据
#define GpsSerial Serial
#define DebugSerial Serial 
#define smokepin  A2 //测量A0引脚的电压，即烟雾传感器的数值
#define buzzerpin A5
#define ONE_WIRE_BUS 11 
struct//定义用于储存gps信息的结构体
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
OneWire  ds(11);  // 连接arduino11引脚
SMSGSM sms;
InetGSM inet;
int numdata;
boolean started = false;
int i = 0;
char msg[50];
char datastr[50];
float value;
char* valuestr;
float tem;
char *temstr;
char BUFFER[10];
char Buffer[10];
int L = 13; //LED指示灯引脚
const unsigned int gpsRxBufferLength = 300;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
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
  pinMode(buzzerpin, OUTPUT);
  if (started)
  {
    delay(3000);
    sendSMS("13051680866", "Module Start Work");//模块初始化完成，向用户发送信息
   // httpg("iot.myworkroom.cn", 80, "/");
   //ggps();//lon lat
    delay(1000);
  }
  sensors.begin(); 
};
void loop()
{
  value = getvalue();//获取烟雾传感器测量得到的数值[
  tem=gettem();
  if (value > 100||tem>50)  
  {
    sendSMS("13051680866", "Fire!!!!!!!!!!");//假如测量值大于1000，视为有火灾发生，向用户报警
    digitalWrite(buzzerpin,HIGH);
    delay(10000);
  }
  
  gpsRead();  
  parseGpsBuffer();//获取gps数据//
  printGpsBuffer();


  delay(3000);
  Serial.println(value);
  Serial.println(tem);
  
  valuestr = dtostrf(value, 3, 2, BUFFER);//将浮点数转化为字符串
  temstr=dtostrf(tem,3,2,Buffer);
   Serial.println(valuestr);
  Serial.println(temstr);
  if (Save_Data.isUsefull)
    sprintf(datastr, "/smoke/upload/%s-%s-%s-%s", valuestr,temstr, Save_Data.longitude, Save_Data.latitude);//能接收gps信号，构建含gps数据和烟雾传感器所测数据的字符串
  else
    sprintf(datastr, "/smoke/upload/%s-%s", valuestr,temstr);//能接收gps信号，构建只含烟雾传感器数据的字符串
  Serial.print(F("DataString:"));
  Serial.println(datastr);//在串口打上传的字符串，用于调试
  httpg("iot.myworkroom.cn", 80, datastr);//上传数据
  //gsm.SimpleRead();//查看是否上传成功
  delay(3000);
}


void sendSMS(char* number, char* message)//发送短信的函数
{
  if (sms.SendSMS(number, message))
    Serial.println(F("Message Send Success"));
}

void ggps()//测量gps数据的函数
{
  gpsRead();  
  parseGpsBuffer();
}
void httpg(char* host, int port, char* path)//采用http协议上传，参数host为主机地址（或域名），port服务端口，path为访问路径
{
  if (inet.attachGPRS("cmnet", "", ""))//cmnet移动接入点,3gnet是联通的接入点
    Serial.println(F("GPRS Attached"));
  else Serial.println(F("gprs eorro"));
  delay(1000);
  numdata = inet.httpGET(host, port, path, msg, 50);//用get请求上传数据
  if(numdata)
    Serial.println(F("Http Get Request Success"));
}
float getvalue()//获取烟雾传感器数值函数
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
float gpsRead() //读取GPS串口的信息
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

void printGpsBuffer()//打印gps信息 
{
 if (!Save_Data.isParseData)
      DebugSerial.println("GPS Data is not usefull!");//当无法接收到GPS信息时，打印“GPS DATA is not usefull!”
}

float gettem()
{
  float celsius;
  sensors.requestTemperatures(); 
  celsius=sensors.getTempCByIndex(0);
  return celsius;

}





