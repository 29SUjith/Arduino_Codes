#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define DHTPIN 15           // DHT11 sensor pin
#define RAIN_PIN 34         // Rain sensor analog pin
#define DHTTYPE DHT11

// WiFi Credentials (for Access Point)
const char* ssid = "weather";
const char* password = "12345678";

// Initialize server
WebServer server(80);  // ESP32 server on port 80

// Initialize DHT and LCD
DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // I2C LCD Address (0x27 or 0x3F)

// Variables to store sensor values
float temperature = 0.0;
float humidity = 0.0;
int rainValue = 0;
int rainPercent = 0;
String rainStatus = "";

// Function to read sensor values
void readSensors() {
  temperature = dht.readTemperature();
  humidity = dht.readHumidity();
  rainValue = analogRead(RAIN_PIN);

  // Map rain value to percentage (0% = dry, 100% = max rain)
  rainPercent = map(rainValue, 4095, 0, 0, 100);

  // Determine rain status
  if (rainPercent > 70) {
    rainStatus = "Heavy Rain";
  } else if (rainPercent > 30) {
    rainStatus = "Light Rain";
  } else {
    rainStatus = "No Rain";
  }
}

// Function to display data on LCD
void displayOnLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("T: ");
  lcd.print(temperature);
  lcd.print("C H: ");
  lcd.print(humidity);
  lcd.print("%");

  lcd.setCursor(0, 1);
  lcd.print("Rain: ");
  lcd.print(rainPercent);
  lcd.print("% ");
  lcd.print(rainStatus);
}

// Function to generate HTML webpage
String generateHTML() {
  String html = "<!DOCTYPE html>";
  html += "<html lang='en'><head>";
  html += "<meta charset='UTF-8'><meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>ESP32 Sensor Data</title>";
  html += "<style>";
  html += "body { font-family: Arial, sans-serif; background: #f2f2f2; color: #333; text-align: center; padding: 20px; }";
  html += "h1 { color: #007BFF; }";
  html += "div { background: white; border-radius: 10px; box-shadow: 0 0 10px #aaa; padding: 20px; display: inline-block; }";
  html += "footer { margin-top: 20px; font-size: 14px; color: #666; }";
  html += "</style>";
  html += "<script>setTimeout(() => { window.location.reload(); }, 5000);</script>";  // Auto-refresh
  html += "</head>";
  html += "<body>";
  html += "<h1>ESP32 Sensor Data</h1>";
  html += "<div>";
  html += "<p><strong>Temperature:</strong> " + String(temperature) + " °C</p>";
  html += "<p><strong>Humidity:</strong> " + String(humidity) + " %</p>";
  html += "<p><strong>Rain Status:</strong> " + rainStatus + "</p>";
  html += "<p><strong>Rain Percentage:</strong> " + String(rainPercent) + " %</p>";
  html += "</div>";
  html += "<footer>Updated every 5 seconds</footer>";
  html += "</body></html>";
  
  return html;
}

// Function to handle HTTP requests
void handleRoot() {
  readSensors();
  displayOnLCD();
  server.send(200, "text/html", generateHTML());
}

void setup() {
  Serial.begin(115200);

  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Starting...");

  // **Access Point Mode**
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.println("Access Point Started");
  Serial.print("ESP32 IP Address: ");
  Serial.println(IP);

  lcd.clear();
  lcd.print("ESP32 AP");
  lcd.setCursor(0, 1);
  lcd.print(IP);

  // Start server
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
}
