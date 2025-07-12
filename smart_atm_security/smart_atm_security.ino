#include <MFRC522v2.h>
#include <MFRC522DriverSPI.h>
#include <MFRC522DriverPinSimple.h>
#include <MFRC522Debug.h>
#include <ESP32Servo.h>

#define SS_PIN 5     // Chip Select pin for SPI communication
#define SERVO_PIN 15  // Servo connected to pin 15

MFRC522DriverPinSimple ss_pin(SS_PIN);
MFRC522DriverSPI driver{ss_pin};    // Create SPI driver
MFRC522 mfrc522{driver};            // Create MFRC522 instance
Servo myServo;                      // Servo object 

void setup() {
  Serial.begin(115200);  
  while (!Serial);       // Wait until Serial is ready (for ATMEGA32U4 boards)

  mfrc522.PCD_Init();    // Initialize MFRC522 module
  MFRC522Debug::PCD_DumpVersionToSerial(mfrc522, Serial);  // Display version info

  Serial.println(F("Scan PICC to see UID"));

  // Attach servo to pin 15
  myServo.attach(SERVO_PIN);
}

void loop() {
  // Check for a new card
  if (!mfrc522.PICC_IsNewCardPresent() || !mfrc522.PICC_ReadCardSerial()) {
    delay(100);   // Add a small delay to avoid rapid looping
    return;
  }

  // Print UID
  Serial.print("Card UID: ");
  MFRC522Debug::PrintUID(Serial, mfrc522.uid);
  Serial.println();

  // Store UID in a string
  String uidString = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    if (mfrc522.uid.uidByte[i] < 0x10) {
      uidString += "0"; 
    }
    uidString += String(mfrc522.uid.uidByte[i], HEX);
  }
  
  Serial.println("UID String: " + uidString);

  // Halt the card and stop communication
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  
  delay(500);   // Add delay to prevent continuous reading of the same card
}
