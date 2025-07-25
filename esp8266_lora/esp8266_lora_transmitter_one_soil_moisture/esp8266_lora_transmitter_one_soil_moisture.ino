#include <SPI.h>
#include <LoRa.h>

#define SOIL_PIN A0
#define NSS 15     // D8
#define RST 16     // D0
#define DIO0 5     // D1

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize LoRa
  LoRa.setPins(NSS, RST, DIO0);
  if (!LoRa.begin(433E6)) {  // Set frequency (433 MHz for India)
    Serial.println("Starting LoRa failed!");
    while (1);
  }
  Serial.println("LoRa Transmitter Ready");
}

void loop() {
  int soilValue = analogRead(SOIL_PIN);  // 0 - 1023
  Serial.print("Soil Moisture: ");
  Serial.println(soilValue);

  // Send packet
  LoRa.beginPacket();
  LoRa.print(soilValue);
  LoRa.endPacket();

  Serial.println("Data Sent!");
  delay(2000);  // Send every 2 seconds
}
