#include <LoRa.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>
#include <SPI.h>
#include <math.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <Ticker.h>

// Wi-Fi credentials and server details
const char *ssid = "Project";
const char *password = "12345678";
const char *server = "https://nvs-krishi-pragya.onrender.com/api/core/sensors/";
const String apiKey = "pk-nvs-=13ade28046bc46b78a2b336d05132910=";

// Pin definitions for LoRa - adjusted to avoid reserved pins
#define LORA_NSS 5
#define LORA_RST 26  // Changed from 2 (which is reserved) to 26
#define LORA_DIO0 25 // Changed from -1 to 25 for better functionality

// Pin definitions for TFT display
#define TFT_DC 12
#define TFT_CS 13
#define TFT_MOSI 14
#define TFT_CLK 27
#define TFT_RST 33  // Changed from 0 to 33 to avoid reserved pins
#define TFT_MISO 0  // This could be kept at 0 if not actively used for MISO

// Button definitions
#define BUTTON_PIN 15
#define FOLDER_CHANGE_BUTTON_PIN 4

// Create TFT display object
Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST);

// Meter dimensions
#define METER_RADIUS 30
#define METER_CENTER_X 80
#define METER_CENTER_Y 70
#define VALUE_Y_OFFSET 105

// Sensor data storage
int sensorValues[12] = {0};
int currentSensor = 1;
bool buttonPressed = false;
bool folderChangeButtonPressed = false;

// Folder variables (kept for future implementation)
int currentFolder = 1;

// Create Ticker object
Ticker watchdogTicker;

// Function to reset watchdog timer
void resetWatchdog()
{
    // Add logic to reset the watchdog timer if needed
    // This example does not use an actual hardware watchdog timer.
    // For a hardware watchdog, you would add code to reset it here.
}

void setup()
{
    Serial.begin(115200);
    
    // Wait for serial to initialize
    delay(1000);
    Serial.println("Starting setup...");
    
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(FOLDER_CHANGE_BUTTON_PIN, INPUT_PULLUP);

    // Initialize the display first
    Serial.println("Initializing display...");
    tft.initR(INITR_BLACKTAB);
    tft.setRotation(3);
    tft.fillScreen(ST7735_BLACK);
    tft.setTextColor(ST7735_RED);
    tft.setTextSize(2);
    tft.setCursor(0, 0);
    tft.println("Starting...");
    
    // Initialize LoRa
    Serial.println("Initializing LoRa...");
    LoRa.setPins(LORA_NSS, LORA_RST, LORA_DIO0);
    
    int loraAttempts = 0;
    while (!LoRa.begin(433E6) && loraAttempts < 5) {
        Serial.println("LoRa initialization failed. Retrying...");
        loraAttempts++;
        delay(1000);
    }
    
    if (loraAttempts >= 5) {
        Serial.println("Failed to start LoRa after multiple attempts!");
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0, 0);
        tft.println("LoRa Failed!");
        delay(3000);
    } else {
        Serial.println("LoRa initialized successfully");
    }
    
    // Update display
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
    tft.println("LoRa Receiver");
    
    // Initialize Wi-Fi
    Serial.println("Connecting to WiFi...");
    tft.setCursor(0, 40);
    tft.println("Connecting WiFi");
    WiFi.begin(ssid, password);
    connectToWiFi();

    // Set up Ticker to call resetWatchdog() every 5 seconds
    watchdogTicker.attach(5, resetWatchdog);
    
    // Ready to go
    tft.fillScreen(ST7735_BLACK);
    tft.setCursor(0, 0);
    tft.println("Ready");
    Serial.println("Setup completed");
}

void loop()
{
    // Check for received LoRa packets
    int packetSize = LoRa.parsePacket();
    if (packetSize)
    {
        String packet = "";
        while (LoRa.available())
        {
            packet += (char)LoRa.read();
        }
        Serial.print("Received packet: ");
        Serial.println(packet);

        // Parse sensor values and display them
        parseSensorData(packet);
        sendDataToServer();
        displaySensorValue();
    }

    // Handle button press with debouncing
    handleButtonPress();

    // Handle folder change button press (kept for future implementation)
    handleFolderChange();
}

void displaySensorValue()
{
    // Clear the screen and display the meter for the current sensor
    tft.fillScreen(ST7735_BLACK);

    String sensorName = "Sensor" + String(currentSensor);
    drawRoundMeter(sensorValues[currentSensor - 1], sensorName);
}

void drawRoundMeter(int value, String sensorName)
{
    // Draw meter background
    tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS, ST7735_WHITE);
    tft.drawCircle(METER_CENTER_X, METER_CENTER_Y, METER_RADIUS + 1, ST7735_WHITE);

    // Draw filled arc based on sensor value
    int startAngle = -90;
    int endAngle = map(value, 0, 100, -90, 90); // Fixed range from -90 to 90 degrees
    for (int angle = startAngle; angle <= endAngle; angle++)
    {
        float angleRad = radians(angle);
        int x = METER_CENTER_X + METER_RADIUS * cos(angleRad);
        int y = METER_CENTER_Y + METER_RADIUS * sin(angleRad);
        tft.drawLine(METER_CENTER_X, METER_CENTER_Y, x, y, ST7735_GREEN);
    }

    // Draw sensor name and value text
    tft.setCursor(0, 0);
    tft.setTextColor(ST7735_YELLOW);
    tft.setTextSize(2);
    tft.print(sensorName);

    tft.setCursor(0, VALUE_Y_OFFSET);
    tft.setTextColor(ST7735_WHITE);
    tft.setTextSize(2);
    tft.print("Value: ");
    tft.print(value);
    tft.print("%");
}

void sendDataToServer()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        Serial.println("Sending data to server...");
        HTTPClient http;
        http.begin(server);
        http.addHeader("Content-Type", "application/json");
        http.addHeader("Authorization", "DeviceToken " + apiKey);

        StaticJsonDocument<256> doc;
        doc["temperature"] = "0";
        doc["humidity"] = "0";
        JsonObject soilMoisture = doc.createNestedObject("soil_moisture");
        for (int i = 0; i < 12; i++)
        {
            soilMoisture["s" + String(i + 1)] = sensorValues[i];
        }
        doc["nitrogen"] = "0";
        doc["phosphorus"] = "0";
        doc["potassium"] = "0";

        String requestBody;
        serializeJson(doc, requestBody);
        
        Serial.println("Request body: " + requestBody);
        
        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode > 0)
        {
            String response = http.getString();
            Serial.println("Response code: " + String(httpResponseCode));
            Serial.println("Response: " + response);
        }
        else
        {
            Serial.print("Error on sending POST: ");
            Serial.println(httpResponseCode);
            Serial.print("Error detail: ");
            Serial.println(http.errorToString(httpResponseCode));
        }

        http.end();
    }
    else
    {
        Serial.println("WiFi Disconnected, attempting to reconnect...");
        WiFi.begin(ssid, password);
        connectToWiFi();
    }
}

void parseSensorData(String packet)
{
    Serial.println("Parsing sensor data...");
    for (int i = 0; i < 12; i++)
    {
        String sensorLabel = "Sensor" + String(i + 1) + ":";
        int sensorStartIndex = packet.indexOf(sensorLabel);
        if (sensorStartIndex != -1)
        {
            int sensorEndIndex = packet.indexOf(';', sensorStartIndex);
            if (sensorEndIndex != -1) {
                String valueStr = packet.substring(sensorStartIndex + sensorLabel.length(), sensorEndIndex);
                valueStr.trim();
                int value = valueStr.toInt();
                sensorValues[i] = value;
                Serial.println("Sensor " + String(i + 1) + ": " + String(value));
            }
        }
    }
}

void handleButtonPress()
{
    if (digitalRead(BUTTON_PIN) == LOW)
    {
        if (!buttonPressed)
        {
            currentSensor = (currentSensor % 12) + 1;
            buttonPressed = true;
            Serial.println("Button pressed. Changing to Sensor " + String(currentSensor));
            displaySensorValue();
            delay(50); // Debounce delay
        }
    }
    else
    {
        buttonPressed = false;
    }
}

void handleFolderChange()
{
    if (digitalRead(FOLDER_CHANGE_BUTTON_PIN) == LOW)
    {
        if (!folderChangeButtonPressed)
        {
            currentFolder = (currentFolder % 3) + 1;
            Serial.print("Current folder changed to: ");
            Serial.println(currentFolder);
            folderChangeButtonPressed = true;
            delay(50); // Debounce delay
        }
    }
    else
    {
        folderChangeButtonPressed = false;
    }
}

void connectToWiFi()
{
    int wifiTimeout = 0;
    Serial.println();
    Serial.print("Connecting to WiFi ");
    Serial.println(ssid);
    
    while (WiFi.status() != WL_CONNECTED && wifiTimeout < 20)
    {
        delay(500);
        Serial.print(".");
        wifiTimeout++;
    }
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("Connected to WiFi.");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
        
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0, 0);
        tft.println("WiFi Connected");
        tft.setCursor(0, 40);
        tft.println(WiFi.localIP().toString());
        delay(2000); 
    } else {
        Serial.println();
        Serial.println("Failed to connect to WiFi. Continuing without connection.");
        
        tft.fillScreen(ST7735_BLACK);
        tft.setCursor(0, 0);
        tft.println("WiFi Failed");
        delay(2000);
    }
}