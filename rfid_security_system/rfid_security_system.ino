#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 rfid(SS_PIN, RST_PIN); // Instance of the class

MFRC522::MIFARE_Key key; 
int relay=8;

// Define the valid UID (for example, your provided UID: 83 EA 6C 14)
byte validUID[] = { 0xBD, 0xFE, 0x45, 0x21 };  // Replace this with the UID of your allowed card

void setup() {
  Serial.begin(9600);
  SPI.begin();  // Init SPI bus
  rfid.PCD_Init();  // Init RC522 
  pinMode(relay,OUTPUT);
  digitalWrite(relay,HIGH);
}

void loop() {
  int potvalue = analogRead(A0);  // Read potentiometer
  long outputValue = map(potvalue, 0, 1023, 0, 60); // Map to minutes

  long potvalue1 = outputValue * 60000; // Convert minutes to milliseconds

  Serial.print("Delay in milliseconds: ");
  Serial.println(potvalue1);
  Serial.print("Delay in minutes: ");
  Serial.println(outputValue);
  delay(1000);


  // Check for new card detection at the beginning of the loop
  if ( ! rfid.PICC_IsNewCardPresent()) {
    return; // No new card present, just skip to the next loop iteration
  }

  // Verify if the NUID has been read
  if ( ! rfid.PICC_ReadCardSerial()) {
    return; // Could not read the card serial, skip to next iteration
  }

  // Print RFID UID for debugging
  Serial.print(F("RFID Tag UID:"));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println("");

  // Check if the detected UID matches the valid UID
  if (isUIDValid(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Charging...");
    Serial.println("Valid RFID Tag detected.");

    digitalWrite(relay,LOW);
    delay(potvalue1);
    digitalWrite(relay,HIGH);
    Serial.println("Charging complete");
    
  } else {
    // If the UID is not valid, do nothing (servo will stay at 0 degrees)
    Serial.println("Invalid RFID Tag detected.");
    digitalWrite(relay,HIGH);
  }

  // Halt PICC after processing the card
  //rfid.PICC_HaltA();  // Halt the card to end communication

  // Prepare the reader for the next card detection
  rfid.PICC_ReadCardSerial();  // Reset the reader to scan for new cards
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
