#include <HttpPacket.h>
#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6,7); // RX, TX
#endif

char ssid[] = "WIFI";            // your network SSID (name)
char pass[] = "1234567890";        // your network password
int status = WL_IDLE_STATUS;     // the Wifi radio's status

char server[] = "api.heclouds.com";
HttpPacketHead packet;
float i=0;//上传的数据
char buff[10];
// Initialize the Ethernet client object
WiFiEspClient client;

void setup()
{
  // initialize serial for debugging
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
  // initialize serial for ESP module
  Serial1.begin(9600);
  // initialize ESP module
  WiFi.init(&Serial1);

  // check for the presence of the shield
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    // don't continue
    while (true);
  }

  // attempt to connect to WiFi network
  while ( status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    status = WiFi.begin(ssid, pass);
  }

  // you're connected now, so print out the data
  Serial.println("You're connected to the network");
  
  printWifiStatus();

  Serial.println();
 // Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
 // if (client.connect(server, 80)) {
   // Serial.println("Connected to server");
    // Make a HTTP request
    //client.println("GET /asciilogo.txt HTTP/1.1");
   // client.println("Host: arduino.cc");
    //client.println("Connection: close");
    //client.println();
  }

void loop()
{
  // if there are incoming bytes available
  // from the server, read them and print them
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }
  //格式化字符串
  char *value;
  value = dtostrf(i, 3, 2, buff);
  char p[100];
  sprintf(p,"{\"datastreams\":[{\"id\":\"temperature\",\"datapoints\":[{\"value\":%s}]}]}",value);
  packet.setHostAddress("api.heclouds.com");
  packet.setDevId("29982897");
  packet.setAccessKey("RbDdq8h3nqHrA5Q86UZdWkdMBP8=");
  /*create the http message about add datapoint ，生成http数据包，用于上传数据*/
  packet.createCmdPacket(POST, TYPE_DATAPOINT, p);
  if (strlen(packet.content))
    Serial.print(packet.content);
  Serial.print(p);
  Serial.println("\n");
 Serial.println("Starting connection to server...");
  // if you get a connection, report back via serial
 if (client.connect(server, 80)) {
 Serial.println("Connected to server");
    // Make a HTTP request
client.println(p);
}
//client.println("Host: arduino.cc");
//client.println("Connection: close");
//client.println();
  // if the server's disconnected, stop the client
  if (!client.connected()) {
    Serial.println();
    Serial.println("Disconnecting from server...");
    client.stop();

    // do nothing forevermore
    //while (true);
    i++;
  }
}


void printWifiStatus()
{
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
}
