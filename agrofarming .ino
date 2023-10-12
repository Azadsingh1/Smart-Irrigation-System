#include <ESP8266WiFi.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>  // Including library for dht
 
#define DHTPIN D3          //pin where the dht11 is connected
DHT dht(DHTPIN, DHT11);
#define rainDigital D1
 
String apiKey = "9IX5DUXMOOJW6MLJ"; // Enter your Write API key from ThingSpeak
const char *ssid = "POCO X2";     // replace with your wifi ssid and wpa2 key
const char *pass = "AndroidAP";
const char* server = "api.thingspeak.com";
 
 
const int AirValue = 790;   //you need to replace this value with Value_1
const int WaterValue = 390;  //you need to replace this value with Value_2
const int SensorPin = A0;
int soilMoistureValue = 0;
int soilmoisturepercent=0;
int relaypin = D5;
int relaypin2 = D6;

 
WiFiClient client;
 
 
void setup() {
  Serial.begin(115200); // open serial port, set the baud rate to 9600 bps
  pinMode(relaypin, OUTPUT);
  pinMode(relaypin2, OUTPUT);
  pinMode(SensorPin,INPUT);
 
  dht.begin();
  
  WiFi.begin(ssid, pass);
 
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
    Serial.println("");
    Serial.println("WiFi connected");
    delay(4000);
}  
 
 
void loop() 
{
  int sensorValue = analogRead(D4);   // read the input on analog pin 0
	float voltage = sensorValue * (5.0 / 1023.0);   // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V)
  float moisture_percentage;
  int sensor_analog;
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  int rainDigitalVal = digitalRead(rainDigital);
  int motorstatus = digitalRead(relaypin);

  sensor_analog = analogRead(SensorPin);
  moisture_percentage = ( 100 - ( (sensor_analog/1023.00) * 100 ) );
  Serial.print("moisture analog = ");
  Serial.println(sensor_analog);
  Serial.print("Moisture Percentage = ");
  Serial.print(moisture_percentage);
  Serial.print("%  ");
 
  Serial.print("Humidity: ");
  Serial.println(h);
  Serial.print("Temperature: ");
  Serial.println(t);
  Serial.print("Rain status: ");
  Serial.println(rainDigitalVal);
  Serial.println(voltage);
  delay(700);


if(sensor_analog>900 && rainDigitalVal == 1 && voltage == 0)
{
  digitalWrite(relaypin, LOW);
  digitalWrite(relaypin2, LOW);
  Serial.println("Motor is ON");
}
else if(rainDigitalVal == 0 && voltage == 5)
{
  digitalWrite(relaypin, HIGH);
  digitalWrite(relaypin, HIGH);
  Serial.println("Motor is OFF");
}
else
{
  digitalWrite(relaypin, HIGH);
  Serial.println("Motor is OFF");
}

  
if (client.connect(server, 80)) // "184.106.153.149" or api.thingspeak.com
{
  String postStr = apiKey;
  postStr += "&field1=";
  postStr += String(moisture_percentage);
  postStr += "&field2=";
  postStr += String(h);
  postStr += "&field3=";
  postStr += String(t);
  postStr += "&field4=";
  postStr += String(rainDigitalVal);
  postStr += "&field5=";
  postStr += String(motorstatus);
  postStr += "\r\n\r\n\r\n\r\n";
    
  client.print("POST /update HTTP/1.1\n");
  client.print("Host: api.thingspeak.com\n");
  client.print("Connection: close\n");
  client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
  client.print("Content-Type: application/x-www-form-urlencoded\n");
  client.print("Content-Length: ");
  client.print(postStr.length());
  client.print("\n\n");
  client.print(postStr);
   
}
  client.stop(); 
}