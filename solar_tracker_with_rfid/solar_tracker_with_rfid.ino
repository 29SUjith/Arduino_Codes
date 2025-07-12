#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>

// LDR pins and variables
int servopin1 = 5; // left and right
int servopin2 = 3; // up and down
int i = 90;  // Initial position for horizontal servo
int j = 90;  // Initial position for vertical servo
int ldr1, ldr2, ldr3, ldr4;  // LDR sensor readings
int red=7;
int green=8;

// RFID pins and variables
#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);  // Instance of the class
MFRC522::MIFARE_Key key;  // Key for RFID communication
byte validUID[] = { 0x83, 0xEA, 0x6C, 0x14 };  // Replace with your valid UID
Servo myservo1;  // Horizontal servo (left-right)
Servo myservo2;  // Vertical servo (up-down)
Servo myservo3;  // RFID-controlled servo (can be the same or different)

// Setup function
void setup() {
  Serial.begin(9600);

  // Initialize servos
  myservo1.attach(servopin1);
  myservo2.attach(servopin2);
  pinMode(red,OUTPUT);
  pinMode(green,OUTPUT);
  // Initialize RFID
  SPI.begin();
  rfid.PCD_Init();
  myservo3.attach(6);  // Attach RFID-controlled servo to pin 6
  myservo3.write(0);  // Set initial position

  // Initialize LDR sensor readings
  for (int pin = A0; pin <= A6; pin++) {
    pinMode(pin, INPUT);
  }
}

// Main loop
void loop() {
  // Handle RFID detection first
  digitalWrite(red,LOW);
  digitalWrite(green,LOW);
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      // Print RFID UID for debugging
      Serial.print(F("RFID Tag UID:"));
      printHex(rfid.uid.uidByte, rfid.uid.size);
      Serial.println("");

      // Check if UID matches valid one
      if (isUIDValid(rfid.uid.uidByte, rfid.uid.size)) {
        Serial.println("Valid RFID Tag detected.");
        // Move servo when valid RFID detected
        digitalWrite(red,LOW);
        digitalWrite(green,HIGH);
        myservo3.write(180);  // Open the door (move servo to 180 degrees)
        delay(2000);  // Wait for 2 seconds
        myservo3.write(0);  // Close the door (move back to 0 degrees)
        delay(1000);  // Wait for 1 second
      } else {
        Serial.println("Invalid RFID Tag detected.");
        digitalWrite(red,HIGH);
        digitalWrite(green,LOW);
      }
      rfid.PICC_HaltA();  // Halt the current card communication
    }
  }

  // Now handle the LDR sensor control for servos (left-right and up-down)
  ldr1 = analogRead(A0);  // Left LDR
  ldr2 = analogRead(A2);  // Down LDR
  ldr3 = analogRead(A4);  // Right LDR
  ldr4 = analogRead(A6);  // Up LDR

  /*Serial.print("ldr1=");
  Serial.println(ldr1);
  Serial.print("ldr2=");
  Serial.println(ldr2);
  Serial.print("ldr3=");
  Serial.println(ldr3);
  Serial.print("ldr4=");
  Serial.println(ldr4);*/

  // Servo control based on LDR readings
  if (ldr2 > ldr4 && ldr2 > 750 && i > 10) {
    i = i - 2;  // Move the horizontal servo left
  }
  else if (ldr4 > ldr2 && ldr4 > 750 && i < 178) {
    i = i + 2;  // Move the horizontal servo right
  }
  else if (ldr1 > ldr3 && ldr1 > 750 && j > 10) {
    j = j - 2;  // Move the vertical servo up
  }
  else if (ldr3 > ldr1 && ldr3 > 750 && j < 178) {
    j = j + 2;  // Move the vertical servo down
  }

  // Update the servo positions
  myservo1.write(i);  // Update horizontal servo position
  myservo2.write(j);  // Update vertical servo position

  delay(100);  // Short delay for responsiveness
}

// Routine to dump a byte array as hex values to Serial
void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

// Function to check if the detected UID matches the valid one
bool isUIDValid(byte *uid, byte uidSize) {
  if (uidSize != sizeof(validUID)) {
    return false;  // If the UID size doesn't match, return false
  }
  
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != validUID[i]) {
      return false;  // If any byte doesn't match, return false
    }
  }
  
  return true;  // UID matches the valid one
}
