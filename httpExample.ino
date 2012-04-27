///////////////LIBRARIES///////////////////
#include <Adafruit_VC0706.h>
#include <SD.h>
#include <SoftwareSerial.h>   

// for wifi
#include <SPI.h>
#include <WiFi.h>

/////////////////////////////
//VARS
//the time we give the sensor to calibrate (10-60 secs according to the datasheet)
int calibrationTime = 30;        

int pirPin = 7;    //the digital pin connected to the PIR sensor's output
int currentState; 
int lastState;
String pictureTaken = "";


//******************************** vars for wifi *****************************//
char ssid[] = "itpsandbox"; //  your network SSID (name) 
char pass[] = "NYU+s0a!+P?";    // your network password (use for WPA, or use as key for WEP)
int status = WL_IDLE_STATUS;

// Initialize the Ethernet client library
// with the IP address and port of the server 
// that you want to connect to (port 80 is default for HTTP):
WiFiClient client;

const char server[] = "courtney-mitchell.com";

boolean readingDate = false;
int theTime = 0;

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 300*1000;  // delay between updates, in milliseconds


//************************* vars for camera**************************************//
#define chipSelect 4 //define SD pin
// On Uno: camera TX connected to pin 2, camera RX to pin 3:
SoftwareSerial cameraconnection = SoftwareSerial(2, 5);
Adafruit_VC0706 cam = Adafruit_VC0706(&cameraconnection);

/////////////////////////////
//SETUP
void setup(){
  Serial.begin(576000);

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    // don't do anything more:
    return;
  } 
  else {
    Serial.println("Card Found!");
  } 

  // locate camera
  if (cam.begin()) {
    Serial.println("Camera Found:");
  } 
  else {
    Serial.println("No camera found?");
    return;
  } 
  while( status != WL_CONNECTED) { 
    status = WiFi.begin(ssid, pass);
    Serial.println("attempting to connect to the internet");
    delay(10000);
  }
  Serial.println("Connected to wifi");
  //printWifiStatus();
  httpRequest("IMAGE10.JPG");

}

////////////////////////////

void loop() {
}
////////////////////////////

void httpRequest(String thisFile) {
  char fileName[13];
  thisFile.toCharArray(fileName, 13);
  File myFile = SD.open(fileName);
  unsigned long contentLength = myFile.size();
  Serial.println(contentLength);
  
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.println("POST /windowsill/fileupload.php HTTP/1.1");
    client.println("Host: www.courtney-mitchell.com");
    client.println("Content-Type: multipart/form-data; boundary=----H4rkNrF ");

    client.print("Content-length: ");
    client.println(contentLength);
    Serial.println(contentLength);
    client.println();

    client.println("------H4rkNrF	");
    client.println(" Content-Disposition: form-data; name=\"submit\"");
    client.println();
    client.println("Upload ");
    client.println("------H4rkNrF ");
    client.print("Content-Disposition: form-data; name=\"file\"; filename=");
    client.println(fileName);
    client.println("Content-Type: image/jpeg");

  int bytes = 0;
    //[actual bytes of the file go here
    // read from the file until there's nothing else in it:
    while (myFile.available()) {
      client.write(myFile.read());
      bytes++;
      Serial.println(bytes);
    }
    // close the file:
    myFile.close();

    client.println("------H4rkNrF--");



    // note the time that the connection was made:
    lastConnectionTime = millis();
  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println("disconnecting.");
    client.stop();
  }
}


