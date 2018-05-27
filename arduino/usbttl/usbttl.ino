#include <SoftwareSerial.h>

SoftwareSerial mySerial(8, 9); // RX, TX

void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }


  Serial.println("Goodnight moon!");

  // set the data rate for the SoftwareSerial port
  mySerial.begin(4800);
  mySerial.println("AT");
}

void loop() { // run over and over
  if (mySerial.available()) {
    delay(100);
    Serial.write(mySerial.read());
  }
  if (Serial.available()) {
     delay(100);
    mySerial.write(Serial.read());
  }
}
sendcmd()
