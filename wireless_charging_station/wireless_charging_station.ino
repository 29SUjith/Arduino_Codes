#include <LiquidCrystal_I2C.h>

// Pin definitions
#define IR_SENSOR_PIN 7
#define RELAY_PIN 6

// I2C LCD setup (address 0x27, 16x2 display)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Charging parameters
const float CHARGING_RATE_PER_MINUTE = 20.0; // Cost per minute in your currency
const float POWER_CONSUMPTION = 10.0; // Watts (adjust based on your charger)

// Variables
bool carDetected = false;
bool chargingActive = false;
unsigned long startTime = 0;
unsigned long currentTime = 0;
unsigned long chargingDuration = 0;
float totalCost = 0.0;
float totalEnergy = 0.0; // in Wh (Watt-hours)

void setup() {
  // Initialize serial communication
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(IR_SENSOR_PIN, INPUT_PULLUP);
  pinMode(RELAY_PIN, OUTPUT);
  
  // Initialize relay to OFF state (HIGH for active low relay)
  digitalWrite(RELAY_PIN, HIGH);
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  
  // Display welcome message
  lcd.setCursor(0, 0);
  lcd.print("Car Charging");
  lcd.setCursor(0, 1);
  lcd.print("Station Ready");
  
  delay(2000);
  lcd.clear();
  
  Serial.println("Wireless Car Charging Station Initialized");
}

void loop() {
  // Read IR sensor (assuming LOW when object detected)
  bool sensorState = !digitalRead(IR_SENSOR_PIN); // Invert for active low
  
  // Car detection logic
  if (sensorState && !carDetected) {
    // Car just arrived
    carDetected = true;
    chargingActive = true;
    startTime = millis();
    totalCost = 0.0;
    totalEnergy = 0.0;
    
    // Turn on relay (LOW for active low relay)
    digitalWrite(RELAY_PIN, LOW);
    
    Serial.println("Car detected - Charging started");
    
    // Display charging started message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Charging Started");
    delay(1000);
  }
  
  else if (!sensorState && carDetected) {
    // Car just left
    carDetected = false;
    chargingActive = false;
    
    // Turn off relay (HIGH for active low relay)
    digitalWrite(RELAY_PIN, HIGH);
    
    Serial.println("Car removed - Charging stopped");
    Serial.print("Total charging time: ");
    Serial.print(chargingDuration / 60000.0, 2);
    Serial.println(" minutes");
    Serial.print("Total cost: Rs=");
    Serial.println(totalCost, 2);
    
    // Display final bill
    displayFinalBill();
    delay(5000);
    
    // Reset display
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Station Ready");
  }
  
  // Update charging information if actively charging
  if (chargingActive && carDetected) {
    currentTime = millis();
    chargingDuration = currentTime - startTime;
    
    // Calculate cost based on time (minutes)
    float minutes = chargingDuration / 60000.0;
    totalCost = minutes * CHARGING_RATE_PER_MINUTE;
    
    // Calculate energy consumption (Watt-hours)
    totalEnergy = (minutes / 60.0) * POWER_CONSUMPTION;
    
    // Update display
    updateChargingDisplay(minutes);
  }
  
  // Small delay to prevent excessive processing
  delay(500);
}

void updateChargingDisplay(float minutes) {
  lcd.clear();
  
  // First line: Time and Status
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (minutes < 1.0) {
    lcd.print((int)(minutes * 60));
    lcd.print("s");
  } else {
    lcd.print(minutes, 1);
    lcd.print("m");
  }
  lcd.print(" CHRG");
  
  // Second line: Cost
  lcd.setCursor(0, 1);
  lcd.print("Cost: Rs=");
  lcd.print(totalCost, 2);
  
  // Optional: Print to serial for debugging
  if ((int)minutes % 1 == 0) { // Print every minute
    Serial.print("Charging time: ");
    Serial.print(minutes, 1);
    Serial.print(" min, Cost: Rs=");
    Serial.print(totalCost, 2);
    Serial.print(", Energy: ");
    Serial.print(totalEnergy, 2);
    Serial.println(" Wh");
  }
}

void displayFinalBill() {
  lcd.clear();
  
  // Calculate final values
  float finalMinutes = chargingDuration / 60000.0;
  
  // Display final bill
  lcd.setCursor(0, 0);
  lcd.print("Session Complete");
  delay(2000);
  
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  lcd.print(finalMinutes, 1);
  lcd.print(" min");
  
  lcd.setCursor(0, 1);
  lcd.print("Total: Rs=");
  lcd.print(totalCost, 2);
  
  // Optional: Show energy consumed
  delay(3000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Energy Used:");
  lcd.setCursor(0, 1);
  lcd.print(totalEnergy, 1);
  lcd.print(" Wh");
  delay(2000);
}