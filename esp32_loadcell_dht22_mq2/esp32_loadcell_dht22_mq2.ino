#include <Arduino.h>
#include "HX711.h"
#include <DHT.h>

// HX711 Configuration
#define LOADCELL_DOUT_PIN  18
#define LOADCELL_SCK_PIN   19
#define CALIBRATION_FACTOR -70500.0  // Use your calibrated factor
HX711 scale;

// DHT22 Configuration
#define DHT_PIN 27
#define DHT_TYPE DHT22
DHT dht(DHT_PIN, DHT_TYPE);

// MQ2 Configuration
#define MQ2_PIN 34

void setup() {
  Serial.begin(115200);

  // HX711 Initialization
  Serial.println("Initializing Load Cell...");
  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(CALIBRATION_FACTOR);
  scale.tare();  // Reset to zero
  Serial.println("Load Cell Ready.");

  // DHT22 Initialization
  dht.begin();
  Serial.println("DHT22 Ready.");

  // MQ2 Initialization
  pinMode(MQ2_PIN, INPUT);
  Serial.println("MQ2 Ready.");
}

void loop() {
  // Read MQ2 gas sensor
  int mq2Value = analogRead(MQ2_PIN);
  float gasPercentage = (mq2Value / 4095.0) * 100.0;  // Convert to percentage

  // Read DHT22 temperature and humidity
  float temperature = dht.readTemperature();
  float humidity = dht.readHumidity();

  // Read Load Cell Weight
  float weight = scale.get_units(10);  // Average of 10 readings
  if (isnan(weight)) weight = 0.0;     // Handle invalid readings

  // Display Sensor Readings
  Serial.println("\n----- Sensor Readings -----");
  
  // Gas Level
  Serial.print("Gas Level: ");
  Serial.print(gasPercentage, 2);
  Serial.println(" %");

  // Temperature
  Serial.print("Temperature: ");
  Serial.print(temperature, 2);
  Serial.println(" °C");

  // Humidity
  Serial.print("Humidity: ");
  Serial.print(humidity, 2);
  Serial.println(" %");

  // Weight
  Serial.print("Weight: ");
  Serial.print(weight, 2);
  Serial.println(" kg");

  Serial.println("---------------------------");

  delay(2000);  // 2-second delay between readings
}
