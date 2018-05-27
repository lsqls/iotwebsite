#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("AT");
  delay(10000);
  sendcmd("AT+CWMODE=3\r\n");
  sendcmd("AT+CWJAP=\"WIFI\",\"1234567890\"\r\n");
  sendcmd("AT+CIFSR\r\n");
  delay(1000);
 sendcmd("POST /devices/29982897/datapoints?type=3 HTTP/1.1\r\n");
 sendcmd("api-key:Aj9NRt=pYZlyZub4M=uLgsJctdI=\r\n");
 sendcmd("Host:api.heclouds.com\r\n");
 sendcmd("Content-Length:20\r\n\r\n");
 sendcmd("{\"temperature\":100}");
}

void loop() { // run over and over
 
}
void sendcmd(char *cmd)
{  
   mySerial.write(cmd);
   delay(500);
   if (mySerial.available()) {
    delay(100);
    Serial.write(mySerial.read());
  }
}

