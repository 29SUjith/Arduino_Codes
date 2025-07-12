#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <TinyGPSPlus.h>

#define DHTPIN 4
#define DHTTYPE DHT11
#define MQ135PIN 34

DHT dht(DHTPIN, DHTTYPE);
TinyGPSPlus gps;
HardwareSerial GPSserial(2); // Using Serial2

const char* ssid = "Carbon_emission_monitor";
const char* password = "12345678";

WebServer server(80);

void handleRoot() {
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();
  int airQualityRaw = analogRead(MQ135PIN);
  float airQualityPercent = (airQualityRaw / 4095.0) * 100.0;

  String latitude = "Waiting...";
  String longitude = "Waiting...";

  // Read GPS data
  while (GPSserial.available()) {
    gps.encode(GPSserial.read());
  }

  if (gps.location.isValid()) {
    latitude = String(gps.location.lat(), 6);
    longitude = String(gps.location.lng(), 6);
  }

  // HTML Response
  String html = "<!DOCTYPE html><html><head><meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>";
  html += "body { font-family: 'Poppins', sans-serif; background: linear-gradient(135deg, #74ebd5, #ACB6E5); text-align: center; padding: 20px; color: #333; }";
  html += "h1 { color: #fff; margin-bottom: 40px; }";
  html += ".card { background-color: white; padding: 20px; margin: 20px auto; box-shadow: 0 8px 16px rgba(0,0,0,0.3); max-width: 320px; border-radius: 15px; transition: transform 0.3s ease; }";
  html += ".card:hover { transform: scale(1.05); }";
  html += ".temperature { background: linear-gradient(135deg, #ff9a9e, #fad0c4); }";
  html += ".humidity { background: linear-gradient(135deg, #a1c4fd, #c2e9fb); }";
  html += ".airquality { background: linear-gradient(135deg, #fbc2eb, #a6c1ee); }";
  html += ".gps { background: linear-gradient(135deg, #90F7EC, #32CCBC); }";
  html += "p { font-size: 20px; font-weight: bold; }";
  html += "</style></head><body>";

  html += "<h1>ESP32 Sensor Dashboard</h1>";
  html += "<div class='card temperature'><p> Temperature<br>" + String(temperature) + " &#8451;</p></div>";
  html += "<div class='card humidity'><p> Humidity<br>" + String(humidity) + " %</p></div>";
  html += "<div class='card airquality'><p> Air Quality<br>" + String(airQualityPercent, 1) + " %</p></div>";
  html += "<div class='card gps'><p> Latitude:<br>" + latitude + "</p><p>Longitude:<br>" + longitude + "</p></div>";

  html += "</body></html>";

  server.send(200, "text/html", html);
}

void setup() {
  Serial.begin(115200);
  dht.begin();
  GPSserial.begin(9600, SERIAL_8N1, 16, 17); // RX = 16, TX = 17

  WiFi.softAP(ssid, password);
  Serial.println("ESP32 AP Started");
  Serial.print("IP: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
