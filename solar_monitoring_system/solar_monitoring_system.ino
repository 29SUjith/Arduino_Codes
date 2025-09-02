  #include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD setup (I2C address might be 0x27 or 0x3F)
// If unsure, run an I2C scanner first
LiquidCrystal_I2C lcd(0x27, 16, 2);

const int analogPin = 34;   // ESP32 ADC pin
float calibrationFactor = 5.7;  // Adjust this after checking with multimeter

void setup() {
  Serial.begin(115200);
  lcd.init();        // Initialize LCD
  lcd.backlight();   // Turn on backlight
  lcd.setCursor(0, 0);
  lcd.print("Solar Monitor");
  delay(1500);
  lcd.clear();
}

void loop() {
  int rawADC = analogRead(analogPin);          // Read ADC value (0 - 4095)
  float voltage = (rawADC * 3.3) / 4095.0;     // Convert ADC value to pin voltage
  float actualVoltage = voltage * calibrationFactor; // Scale to actual voltage

  // Print on Serial Monitor (for debugging / Serial Plotter)
  Serial.println(actualVoltage);

  // Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Solar Volt: ");
  lcd.print(actualVoltage, 2); // 2 decimal places
  lcd.print("  "); // Clear any extra char


  delay(500); // Update twice per second
}
