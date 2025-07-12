#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------- CONFIG ----------
#define DHTPIN D4
#define DHTTYPE DHT11
#define OLED_RESET -1
#define BUTTON_PIN D3
#define BUZZER_PIN D5
#define MQ5_PIN A0

// Timing constants
#define SENSOR_READ_INTERVAL 2000    // DHT11 needs 2s between reads
#define DISPLAY_UPDATE_INTERVAL 100  // Smooth display updates
#define BUTTON_DEBOUNCE_TIME 300
#define WIFI_TIMEOUT 10000          // 10 second WiFi timeout

// Thresholds
#define TEMP_THRESHOLD 35.0
#define GAS_THRESHOLD_PERCENT 40.0   // Changed to percentage threshold

const char* ssid = "Project";
const char* password = "12345678";

DHT dht(DHTPIN, DHTTYPE);
Adafruit_SSD1306 display(128, 32, &Wire, OLED_RESET);
ESP8266WebServer server(80);

// ---------- STATE ----------
uint8_t screenIndex = 0;
unsigned long lastButtonPress = 0;
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;

float temperature = 0.0;
float humidity = 0.0;
int gasValue = 0;
float gasPercentage = 0.0;  // Added gas percentage variable
bool buzzerState = false;
bool sensorError = false;

// ---------- SETUP ----------
void setup() {
  Serial.begin(115200);
  Serial.println(F("\n=== Weather Station Starting ==="));
  
  // Initialize hardware
  initializeHardware();
  
  // Connect to WiFi with timeout
  connectWiFi();
  
  // Setup web server
  setupWebServer();
  
  // Initial display
  showStartupScreen();
  
  Serial.println(F("=== Setup Complete ==="));
}

// ---------- LOOP ----------
void loop() {
  server.handleClient();
  
  unsigned long currentTime = millis();
  
  // Read sensors at specified interval
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    readSensors();
    checkThresholds();
    lastSensorRead = currentTime;
  }
  
  // Handle button presses
  handleButton();
  
  // Update display at specified interval
  if (currentTime - lastDisplayUpdate >= DISPLAY_UPDATE_INTERVAL) {
    updateDisplay();
    lastDisplayUpdate = currentTime;
  }
  
  // Small delay to prevent watchdog issues
  yield();
}

// ---------- INITIALIZATION ----------
void initializeHardware() {
  dht.begin();
  
  // Try common I2C addresses for 0.91" OLED displays
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 0x3C failed, trying 0x3D"));
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3D)) {
      Serial.println(F("SSD1306 allocation failed"));
      while(1); // Halt if display fails
    }
  }
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
}

void connectWiFi() {
  WiFi.begin(ssid, password);
  Serial.print(F("Connecting to WiFi"));
  
  unsigned long wifiStart = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - wifiStart > WIFI_TIMEOUT) {
      Serial.println(F("\nWiFi connection timeout!"));
      return;
    }
    delay(500);
    Serial.print(F("."));
  }
  
  Serial.println();
  Serial.print(F("WiFi connected. IP: "));
  Serial.println(WiFi.localIP());
}

void setupWebServer() {
  server.on("/", HTTP_GET, handleWebpage);
  server.on("/api/data", HTTP_GET, handleAPIData);
  server.begin();
  Serial.println(F("Web server started"));
}

void showStartupScreen() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(F("Weather Station"));
  display.println(F("Starting..."));
  display.println();
  display.print(F("IP: "));
  display.println(WiFi.localIP());
  display.display();
  delay(2000);
}

// ---------- SENSOR FUNCTIONS ----------
void readSensors() {
  float newTemp = dht.readTemperature();
  float newHumidity = dht.readHumidity();
  
  // Check if readings are valid
  if (isnan(newTemp) || isnan(newHumidity)) {
    sensorError = true;
    Serial.println(F("DHT sensor error!"));
  } else {
    sensorError = false;
    temperature = newTemp;
    humidity = newHumidity;
  }
  
  gasValue = analogRead(MQ5_PIN);
  // Convert analog reading (0-1023) to percentage (0-100%)
  gasPercentage = (gasValue / 1023.0) * 100.0;
  
  // Debug output (remove in production)
  #ifdef DEBUG
  Serial.printf("T: %.1f°C, H: %.1f%%, Gas: %d (%.1f%%)\n", temperature, humidity, gasValue, gasPercentage);
  #endif
}

void checkThresholds() {
  bool shouldBuzz = (!sensorError && temperature > TEMP_THRESHOLD) || gasPercentage > GAS_THRESHOLD_PERCENT;
  
  if (shouldBuzz != buzzerState) {
    buzzerState = shouldBuzz;
    digitalWrite(BUZZER_PIN, buzzerState ? HIGH : LOW);
    
    if (buzzerState) {
      Serial.println(F("ALERT: Threshold exceeded!"));
    }
  }
}

// ---------- INPUT HANDLING ----------
void handleButton() {
  if (digitalRead(BUTTON_PIN) == LOW && 
      millis() - lastButtonPress > BUTTON_DEBOUNCE_TIME) {
    screenIndex = (screenIndex + 1) % 3;
    lastButtonPress = millis();
    
    // Force immediate display update
    lastDisplayUpdate = 0;
  }
}

// ---------- DISPLAY FUNCTIONS ----------
void updateDisplay() {
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  
  if (sensorError) {
    display.println(F("SENSOR ERROR"));
    display.println(F("Check DHT11"));
  } else {
    switch (screenIndex) {
      case 0:
        showTemperatureScreen();
        break;
      case 1:
        showHumidityScreen();
        break;
      case 2:
        showGasScreen();
        break;
    }
  }
  
  // Show alert indicator (adjusted for 32px height)
  if (buzzerState) {
    display.fillCircle(120, 4, 3, SSD1306_WHITE);
  }
  
  display.display();
}

void showTemperatureScreen() {
  display.println(F("Temperature:"));
  display.setTextSize(2);
  display.setCursor(0, 12);
  display.print(temperature, 1);
  display.print(F(" C"));
  
  // Show status on 0.91" display (32px height)
  display.setTextSize(1);
  display.setCursor(0, 24);
  if (temperature > TEMP_THRESHOLD) {
    display.print(F("HIGH!"));
  } else {
    display.print(F("OK"));
  }
}

void showHumidityScreen() {
  display.println(F("Humidity:"));
  display.setTextSize(2);
  display.setCursor(0, 12);
  display.print(humidity, 1);
  display.print(F(" %"));
  
  // Show comfort level
  display.setTextSize(1);
  display.setCursor(0, 24);
  if (humidity < 30) {
    display.print(F("Dry"));
  } else if (humidity > 70) {
    display.print(F("Humid"));
  } else {
    display.print(F("OK"));
  }
}

void showGasScreen() {
  display.println(F("Gas Level:"));
  display.setTextSize(2);
  display.setCursor(0, 12);
  display.print(gasPercentage, 1);  // Display gas percentage with 1 decimal
  display.print(F("%"));            // Add percentage symbol
  
  // Show gas status
  display.setTextSize(1);
  display.setCursor(0, 24);  // Changed from 30 to 24 for better fit
  if (gasPercentage > GAS_THRESHOLD_PERCENT) {
    display.print(F("ALERT!"));
  } else {
    display.print(F("Safe"));
  }
}

// ---------- WEB SERVER ----------
void handleWebpage() {
  String html = F("<!DOCTYPE html><html><head>");
  html += F("<title>Weather Station</title>");
  html += F("<meta name='viewport' content='width=device-width, initial-scale=1'>");
  html += F("<meta http-equiv='refresh' content='5'>");
  html += F("<style>");
  html += F("body{font-family:Arial;margin:20px;background:#f0f0f0}");
  html += F(".card{background:white;padding:20px;margin:10px 0;border-radius:8px;box-shadow:0 2px 4px rgba(0,0,0,0.1)}");
  html += F(".alert{background:#ffebee;border-left:4px solid #f44336}");
  html += F(".value{font-size:24px;font-weight:bold;color:#2196F3}");
  html += F("</style></head><body>");
  
  html += F("<h1>🌡️ Weather & Gas Monitor</h1>");
  
  // Temperature card
  html += F("<div class='card");
  if (!sensorError && temperature > TEMP_THRESHOLD) html += F(" alert");
  html += F("'><h3>Temperature</h3><div class='value'>");
  html += sensorError ? F("Error") : String(temperature, 1) + F("°C");
  html += F("</div></div>");
  
  // Humidity card
  html += F("<div class='card'><h3>Humidity</h3><div class='value'>");
  html += sensorError ? F("Error") : String(humidity, 1) + F("%");
  html += F("</div></div>");
  
  // Gas card - now showing percentage
  html += F("<div class='card");
  if (gasPercentage > GAS_THRESHOLD_PERCENT) html += F(" alert");
  html += F("'><h3>Gas Level</h3><div class='value'>");
  html += String(gasPercentage, 1) + F("%");  // Display percentage instead of raw value
  html += F("</div><p>Raw: ") + String(gasValue) + F("</p>");  // Optional: show raw value too
  html += F("</div>");
  
  html += F("<div class='card'><h3>System Status</h3>");
  html += F("<p>WiFi: Connected (") + WiFi.localIP().toString() + F(")</p>");
  html += F("<p>Uptime: ") + String(millis() / 1000) + F(" seconds</p>");
  html += F("<p>Free Heap: ") + String(ESP.getFreeHeap()) + F(" bytes</p>");
  html += F("</div>");
  
  html += F("</body></html>");
  
  server.send(200, F("text/html"), html);
}

void handleAPIData() {
  String json = F("{");
  json += F("\"temperature\":");
  json += sensorError ? F("null") : String(temperature, 1);
  json += F(",\"humidity\":");
  json += sensorError ? F("null") : String(humidity, 1);
  json += F(",\"gasLevel\":");
  json += String(gasValue);
  json += F(",\"gasPercentage\":");  // Added gas percentage to API
  json += String(gasPercentage, 1);
  json += F(",\"alerts\":{\"highTemp\":");
  json += (temperature > TEMP_THRESHOLD && !sensorError) ? F("true") : F("false");
  json += F(",\"highGas\":");
  json += (gasPercentage > GAS_THRESHOLD_PERCENT) ? F("true") : F("false");  // Use percentage threshold
  json += F("},\"sensorError\":");
  json += sensorError ? F("true") : F("false");
  json += F(",\"uptime\":");
  json += String(millis() / 1000);
  json += F("}");
  
  server.send(200, F("application/json"), json);
}