#include <WiFi.h>

void setup() {
    Serial.begin(115200);
    
    WiFi.mode(WIFI_STA);  // Ensure WiFi is in station mode
    delay(100);  // Small delay for stability

    Serial.print("ESP32 MAC Address: ");
    Serial.println(WiFi.macAddress());  // Print MAC address
}

void loop() {
    // No need to do anything here
}
