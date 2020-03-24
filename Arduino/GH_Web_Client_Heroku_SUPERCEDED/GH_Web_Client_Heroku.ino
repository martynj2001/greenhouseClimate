/*
 WiFiEsp example: WebClientRepeating

 This sketch connects to a web server and makes an HTTP request
 using an Arduino ESP8266 module.
 It repeats the HTTP call each 10 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/

#include "WiFiEsp.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

char ssid[] = "J-Family-Home";            
char pass[] = "Cmarsh=24962695";     
int status = WL_IDLE_STATUS;     // the Wifi radio's status

const char server[] = "stormy-atoll-61189.herokuapp.com";

unsigned long lastConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long postingInterval = 60000; // delay between updates, in milliseconds

// Initialize the Ethernet client object
WiFiEspClient client;

//LEDs
int redPin = 4;
int greenPin = 3;
int bluePin = 2;

void setup()
{
  // initialise LED pins -> OUTPUT
  pinMode(redPin, OUTPUT);
  pinMode(greenPin, OUTPUT);
  pinMode(bluePin, OUTPUT);
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
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

  Serial.println("You're connected to the network");
  
  printWifiStatus();
  
}

void loop()
{
  setColor(0,255,0);
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
    char c = client.read();
    Serial.write(c);
  }

  // if 10 seconds have passed since your last connection,
  // then connect again and send data
  if (millis() - lastConnectionTime > postingInterval) {
    httpRequest();
  }
}

// this method makes a HTTP connection to the server
void httpRequest()
{
  Serial.println();
  
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();
  int temp_out = getSensorData();
  int temp_in = getSensorData();
  String jsonData = "{\"temp_out\":" + String(temp_out)+ ", \"temp_in\":" + String(temp_in) + "}";
  sendSensorData(jsonData);
}


void printWifiStatus()
{
  setColor(0, 255, 0);  // green
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

int getSensorData()
{
  return random(500);
}

void sendSensorData(String value){
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println("POST /measures HTTP/1.1");
    client.println("Host: stormy-atoll-61189.herokuapp.com");
    client.println("Content-type: application/json");
    client.print("Content-length: ");
    client.println(value.length());
    client.println("Connection: keep-alive");
    client.println();
    client.println(value);
    lastConnectionTime = millis();
    Serial.println("Sent to server");
    setColor(0,0,255);
    delay(1000);
  }  
}

void setColor(int red, int green, int blue)
{
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);  
}
