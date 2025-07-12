#include <WiFi.h>
#include <HTTPClient.h>
#include <DHT.h>
#include <Adafruit_BMP085.h>

int ir1=35;
int ir2=32;
int ir3=33;
int ldr1=23;

int led1=5;
int led2=18;
int led3=19;

const char* ssid = "Project";       
const char* password = "12345678"; 

const char* server = "http://api.thingspeak.com/update";
const char* apiKey = "4S5Q9JYSG86OH6OR"; 

#define DHTPIN 15    
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

Adafruit_BMP085 bmp;

#define SWITCH1_PIN 13
#define SWITCH2_PIN 12
#define SWITCH3_PIN 14

#define SOIL_MOISTURE_PIN 34 

void setup() {
  Serial.begin(115200);

  dht.begin();

  if (!bmp.begin()) {
    Serial.println("Could not find BMP180 sensor!");
    while (1);
  }
  pinMode(ir1,INPUT);
  pinMode(ir2,INPUT);
  pinMode(ir3,INPUT);
  pinMode(ldr1,INPUT);
  pinMode(SOIL_MOISTURE_PIN,INPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH3_PIN, INPUT_PULLUP);

  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
}

void loop() {

  int ir1r=digitalRead(ir1);
  int ir2r=digitalRead(ir2);
  int ir3r=digitalRead(ir3);
  int ldr1r=digitalRead(ldr1);


  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  float pressure = bmp.readPressure() / 100.0; 
  float altitude = bmp.readAltitude();

  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  float soilMoisturePercent = map(soilMoisture, 0, 4095, 0, 100); 

  int switch1 = digitalRead(SWITCH1_PIN);
  int switch2 = digitalRead(SWITCH2_PIN);
  int switch3 = digitalRead(SWITCH3_PIN);

  Serial.print("Temperature: "); Serial.println(temperature);
  Serial.print("Humidity: "); Serial.println(humidity);
  Serial.print("Pressure: "); Serial.println(pressure);
  Serial.print("Altitude: "); Serial.println(altitude);
  Serial.print("Soil Moisture: "); Serial.print(soilMoisturePercent); Serial.println("%");
  Serial.print("Switch 1: "); Serial.println(switch1 == LOW ? "Pressed" : "Released");
  Serial.print("Switch 2: "); Serial.println(switch2 == LOW ? "Pressed" : "Released");
  Serial.print("Switch 3: "); Serial.println(switch3 == LOW ? "Pressed" : "Released");

  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = String(server) + "?api_key=" + apiKey +
                 "&field1=" + String(temperature) +
                 "&field2=" + String(humidity) +
                 "&field3=" + String(pressure) +
                 "&field4=" + String(altitude) +
                 "&field5=" + String(soilMoisturePercent) +
                 "&field6=" + String(switch1) +
                 "&field7=" + String(switch2) +
                 "&field8=" + String(switch3);
    http.begin(url);
    int httpResponseCode = http.GET();
    if (httpResponseCode > 0) {
      Serial.print("Data sent to ThingSpeak. HTTP Response Code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("Error sending data to ThingSpeak. HTTP Response Code: ");
      Serial.println(httpResponseCode);
    }
    http.end();
  } else {
    Serial.println("Wi-Fi not connected!");
  }

  if(ldr1r==1){
    if(ir1r==0){
      analogWrite(led1,55);
      analogWrite(led2,55);
      analogWrite(led3,255);
    }
    else if(ir2r==0){
      analogWrite(led1,55);
      analogWrite(led2,255);
      analogWrite(led3,55);
    }
    else if(ir3r==0){
      analogWrite(led1,255);
      analogWrite(led2,55);
      analogWrite(led3,55);
    }
    else{
      analogWrite(led1,55);
      analogWrite(led2,55);
      analogWrite(led3,55);
    }
  }
  else{
    analogWrite(led1,0);
    analogWrite(led2,0);
    analogWrite(led3,0);
  }

  delay(100); 
}