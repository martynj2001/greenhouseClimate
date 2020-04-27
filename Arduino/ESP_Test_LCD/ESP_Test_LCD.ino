

/*
 WiFiEsp example: WebClientRepeating

 This sketch connects to a web server and makes an HTTP request
 using an Arduino ESP8266 module.
 It repeats the HTTP call each 10 seconds.

 For more details see: http://yaab-arduino.blogspot.com/p/wifiesp.html
*/

#include "WiFiEsp.h"
#include <dht.h>

#include <LCD5110_Basic.h>
#include "arduino_secrets.h"
// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX // 7, 6
#endif

char ssid[] = SECRET_SSID;            
char pass[] = SECRET_PASS;     
int status = WL_IDLE_STATUS;  // the Wifi radio's status

const char server[] = "polar-headland-35646.herokuapp.com";

unsigned long lastPOSTConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long POSTInterval = 60000; // delay between updates, in milliseconds (every 15 mins in production)
unsigned long lastGETConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long GETInterval = 20000; // delay between updates, in milliseconds (set higher in production)

// Initialize the Ethernet client object
WiFiEspClient client;
// Initialize the temp sensors
dht dhtOut;
dht dhtIn;
float temp_in, temp_out, hum_in, hum_out, moisture;
// TO-DO - impliment moisture sensor.

//Parse Counter
int counter = 0;

//Soil Moisture Sensor

const int AirValue = 655;   //you need to replace this value with Value_1
const int WaterValue = 307;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
int soilMoistureState = DRY;

//Nokia Screen
LCD5110 myGLCD(47,46,45,43,44);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

void setup()
{
  // initialise LED pins -> OUTPUT
  pinMode(RED_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BLUE_LED, OUTPUT);
  // initialise Pump
  pinMode(PUMP_PIN, OUTPUT);
  // initialize serial for debugging
  Serial.begin(115200);
  // initialize serial for ESP module
  Serial1.begin(115200);
  // initialize ESP module
  WiFi.init(&Serial1);

  //Initilaise screen
  myGLCD.InitLCD();
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(SCREEN_WAKE, INPUT_PULLUP);
  digitalWrite(BACKLIGHT, 1);

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
  //Control the screen backlight and sleep mode
  if (digitalRead(SCREEN_WAKE) == LOW) {
    //Wake Screen & turn on backlight & update information
    digitalWrite(BACKLIGHT, 0);
    //myGLCD.disableSleep();
  }
  else {
    digitalWrite(BACKLIGHT, 1);
    //myGLCD.enableSleep();
  }
  setColor(0,0,0);
  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
   /*while(client.connected())
   {
      // There is a client connected. Is there anything to read?
      while(client.available() > 0)
      {
         char c = client.read();
         Serial.print(c);
      }
      Serial.println();
      Serial.println("Waiting for more data...");
      delay(100); // Have a bit of patience...
   }*/
  while(client.available()) {
    char c = client.read();
    parseHTTPResponse(c);
  }
  // if 10 seconds has passed since last GET request, reconnect and send GET request for pump status
  if (millis() - lastGETConnectionTime > GETInterval) {
    httpGETRequest();
    //if 60 seconds has passed since last system POST request, reconnect and send system status.
    if (millis() - lastPOSTConnectionTime > POSTInterval) {
      delay(1000);
      httpPOSTRequest();
    }
  }
}

// this method makes a HTTP connection to the server
void httpPOSTRequest()
{
  Serial.println();
  
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();
  getSensorData();
  getSoilMoistureData();
  String jsonData = "{\"temp_out\":" + String(temp_out)+ ", \"temp_in\":" + String(temp_in) + ", \"humidity_in\":" + String(hum_in) + ", \"humidity_out\":" + String(hum_out) + ", \"soil_moisture\":" + String(soilMoistureState) + "}";
  sendSensorData(jsonData);
}

// this method makes a HTTP connection to the server
void httpGETRequest()
{
  Serial.println();
    
  // close any connection before send a new request
  // this will free the socket on the WiFi shield
  client.stop();

  // if there's a successful connection
  if (client.connectSSL(server, 443)) {
    Serial.println("Connecting...");
    
    // send the HTTP PUT request
    client.println("GET /pump/pump_status HTTP/1.1");
    client.println("Host: polar-headland-35646.herokuapp.com");
    client.println("Connection: close");
    client.println();

    // note the time that the connection was made
    lastGETConnectionTime = millis();
    Serial.println("Sent GET request to server");
    setColor(255, 0, 0);
    delay(1000);    
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
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

  //Display connection on LCD
  myGLCD.setFont(SmallFont);
  myGLCD.print("Wifi", LEFT, 0);
}

void getSensorData()
{
  int chk = dhtOut.read11(TEMP_OUT_PIN);
  temp_out = dhtOut.temperature;
  hum_out = dhtOut.humidity;

  chk = dhtIn.read11(TEMP_IN_PIN);
  temp_in = dhtIn.temperature;
  hum_in = dhtIn.humidity; 
}

void sendSensorData(String value){
  if (client.connectSSL(server, 443)) {
    Serial.println("Connected to server");
    client.println("POST /measures HTTP/1.1");
    client.println("Host: polar-headland-35646.herokuapp.com");
    client.println("Content-type: application/json");
    client.print("Content-length: ");
    client.println(value.length());
    client.println("Connection: keep-alive");
    client.println();
    client.println(value);
    lastPOSTConnectionTime = millis();
    Serial.println("Sent POST request to server");
    setColor(0,0,255);
    delay(1000);
    
  }  
}

void setColor(int red, int green, int blue)
{
  analogWrite(RED_LED, red);
  analogWrite(GREEN_LED, green);
  analogWrite(BLUE_LED, blue);  
}

static void parseHTTPResponse(char c)
{
  Serial.print(c);
  if(c == '|'|| counter == 1){
    counter++;
    if(counter == 2){
      counter = 0;
    }
    if(c == '1'){
      Serial.println("Turning the pump ON");
      digitalWrite(PUMP_PIN, 1);
    }
     else if (c == '0'){
      Serial.println("Turning the pump OFF");
      digitalWrite(PUMP_PIN, 0);
    }
  }
}

void getSoilMoistureData()
{
  int soilMoistureReading = 0;
  //Get 20 readings & take mean average
  int i;
  for (i = 0; i<=20; i++)
  {
    soilMoistureReading += analogRead(A2);
  }
  soilMoistureReading = soilMoistureReading / 20;

  setSoilMoistureStatus(soilMoistureReading);
  
  Serial.print("Soil Moisture Value is: ");
  Serial.print(soilMoistureReading);
  Serial.print(" - ");
  Serial.println(soilMoistureState);
}

void setSoilMoistureStatus(int soilMoistureReading)
{ 
  if(soilMoistureReading > WaterValue && soilMoistureReading < (WaterValue + intervals))
  {
    soilMoistureState = VERY_WET;
  }
  else if(soilMoistureReading > (WaterValue + intervals) && soilMoistureReading < (AirValue - intervals))
  {
    soilMoistureState = WET;
  }
  else if(soilMoistureReading < AirValue && soilMoistureReading > (AirValue - intervals))
  {
    soilMoistureState = DRY;
  }
}
