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
#include <avr/pgmspace.h>
#include "arduino_secrets.h"

// Emulate Serial1 on pins 6/7 if not present
#ifndef HAVE_HWSERIAL1
#include "SoftwareSerial.h"
SoftwareSerial Serial1(6, 7); // RX, TX
#endif

#if defined(__AVR__)
    #include <avr/pgmspace.h>
    #define imagedatatype const uint8_t
#elif defined(__PIC32MX__)
    #define PROGMEM
    #define imagedatatype const unsigned char
#elif defined(__arm__)
    #define PROGMEM
    #define imagedatatype const unsigned char
#endif

char ssid[] = SECRET_SSID;            
char pass[] = SECRET_PASS;     
int status = WL_IDLE_STATUS;  // the Wifi radio's status

const char server[] = "polar-headland-35646.herokuapp.com";

unsigned long lastPOSTConnectionTime = 0;         // last time you connected to the server, in milliseconds
const unsigned long POSTInterval = 60000;         // delay between updates, in milliseconds (Every Hour - 3600000)
unsigned long lastGETConnectionTime = 0;          // last time you connected to the server, in milliseconds
const unsigned long GETInterval = 5000;          // delay between updates, in milliseconds

// Initialize the Ethernet client object
WiFiEspClient client;

// Initialize the temp sensors
dht dhtOut;
dht dhtIn;
float temp_in, temp_out, hum_in, hum_out, moisture;
float t_temp_in = 0, t_temp_out = 0;

//Initialise the LCD screen
LCD5110 myGLCD(47,46,45,43,44);
extern uint8_t SmallFont[];
extern uint8_t MediumNumbers[];
extern uint8_t BigNumbers[];

//Parse Counter
int counter = 0;

//Soil Moisture Sensor

const int AirValue = 655;   //you need to replace this value with Value_1
const int WaterValue = 307;  //you need to replace this value with Value_2
int intervals = (AirValue - WaterValue)/3;
int soilMoistureState = DRY;

void setup()
{
  // initialise LED pins -> OUTPUT
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  //Initialise LCD Screen
  myGLCD.InitLCD();
  pinMode(BACKLIGHT, OUTPUT);
  pinMode(SCREEN_WAKE, INPUT_PULLUP);
  digitalWrite(BACKLIGHT, 0);
  
  myGLCD.setFont(SmallFont);
  myGLCD.print("Garden Data v3.0", CENTER, 8);
  myGLCD.print("Martyn Jones", CENTER, 24);
  myGLCD.print("Apr.2020", CENTER, 32);
  
  // initialise Pump & Solinoids
  pinMode(PUMP_PIN, OUTPUT);
  
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

  //Get first swwep of data
  getSensorData();
}

void loop()
{
   if (digitalRead(SCREEN_WAKE) == LOW) {
      //Wake Screen & turn on backlight & update information
      digitalWrite(BACKLIGHT, 0);
      getSensorData();
      myGLCD.disableSleep();
      myGLCD.setFont(SmallFont);
      myGLCD.clrScr();
      myGLCD.print(" Air", LEFT, 0);
      myGLCD.print("GH ", RIGHT, 0);
      myGLCD.setFont(MediumNumbers);
      myGLCD.printNumI(temp_out, LEFT, 8);
      myGLCD.printNumI(temp_in, RIGHT, 8);
      myGLCD.setFont(SmallFont);
      myGLCD.print("------------", CENTER, 24);
      myGLCD.print("Wifi: OK", LEFT, 32);
      //Logic to check if pump is on
      myGLCD.print("Water: OFF", LEFT, 40);
      t_temp_in = temp_in;
      t_temp_out = temp_out;
      delay(4000); //Keep screen alive for 5 seconds
    }
    else {
      digitalWrite(BACKLIGHT, 1);
      myGLCD.enableSleep();
    }
  setColor(0,0,0);

  // if there's incoming data from the net connection send it out the serial port
  // this is for debugging purposes only
  while (client.available()) {
    char c = client.read();
    parseHTTPResponse(c);
  }
  // if 10 seconds has passed since last GET request, reconnect and send GET request for pump status
  if (millis() - lastGETConnectionTime > GETInterval) {
    //delay(1000);
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
  setColor(0,0,255);
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
  setColor(255,0,0);
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
  }
  else {
    // if you couldn't make a connection
    Serial.println("Connection failed");
  }
}


void printWifiStatus()
{
  setColor(0, 255, 0);  // green
  myGLCD.clrScr();
  myGLCD.setFont(SmallFont);
  myGLCD.print("Wifi OK", CENTER, 40);
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
  delay(1000);
}

void getSensorData()
{
  int chk = dhtOut.read11(TEMP_OUT_PIN);
  temp_out = dhtOut.temperature;
  hum_out = dhtOut.humidity;

  chk = dhtIn.read11(TEMP_IN_PIN);
  temp_in = dhtIn.temperature;
  hum_in = dhtIn.humidity;
  Serial.print("temp_out = ");
  Serial.println(temp_out);
  Serial.print("temp_in = ");
  Serial.println(temp_in);
   
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
  analogWrite(LED_RED, red);
  analogWrite(LED_GREEN, green);
  analogWrite(LED_BLUE, blue);  
}

static void parseHTTPResponse(char c)
{
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
