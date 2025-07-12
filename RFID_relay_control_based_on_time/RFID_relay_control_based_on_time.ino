#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <MFRC522.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define SS_PIN 10
#define RST_PIN 9
MFRC522 rfid(SS_PIN, RST_PIN);

MFRC522::MIFARE_Key key; 
int relay = 8;
byte validUID[] = { 0xBD, 0xFE, 0x45, 0x21 };

void setup() {
  Serial.begin(9600);

  // Start I2C for OLED
  Wire.begin();
  
  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(10, 10);
  display.println("System Ready");
  display.display();
  
  delay(500); // Give some time before initializing SPI

  // Start SPI for RFID
  SPI.begin();
  rfid.PCD_Init();

  pinMode(relay, OUTPUT);
  digitalWrite(relay, HIGH);
}

void loop() {
  int potvalue = analogRead(A0);
  long outputValue = map(potvalue, 0, 1023, 0, 60);
  long potvalue1 = outputValue * 60000;

  // Display time on OLED
  display.clearDisplay();
  display.setCursor(10, 10);
  display.print("Time:");
  display.print(outputValue);
  display.println("min");
  display.display();

  Serial.print("Delay in milliseconds: ");
  Serial.println(potvalue1);
  Serial.print("Delay in minutes: ");
  Serial.println(outputValue);
  delay(500);  // Reduce delay to ensure RFID reads

  // Check if RFID is detected
  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    return;
  }

  Serial.print(F("RFID Tag UID:"));
  printHex(rfid.uid.uidByte, rfid.uid.size);
  Serial.println("");

  if (isUIDValid(rfid.uid.uidByte, rfid.uid.size)) {
    Serial.println("Charging...");
    Serial.println("Valid RFID Tag detected.");
    
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Charging...");
    display.display();

    digitalWrite(relay, LOW);
    delay(potvalue1);
    digitalWrite(relay, HIGH);
    
    Serial.println("Charging complete");

    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Charge Done!");
    display.display();
    
  } else {
    Serial.println("Invalid RFID Tag detected.");
    display.clearDisplay();
    display.setCursor(10, 10);
    display.println("Invalid Card");
    display.display();
    
    digitalWrite(relay, HIGH);
  }

  rfid.PICC_HaltA();  // Halt the card
  rfid.PCD_StopCrypto1(); // Stop encryption on PCD
}

void printHex(byte *buffer, byte bufferSize) {
  for (byte i = 0; i < bufferSize; i++) {
    Serial.print(buffer[i] < 0x10 ? " 0" : " ");
    Serial.print(buffer[i], HEX);
  }
}

bool isUIDValid(byte *uid, byte uidSize) {
  if (uidSize != sizeof(validUID)) return false;
  
  for (byte i = 0; i < uidSize; i++) {
    if (uid[i] != validUID[i]) return false;
  }
  
  return true;
}
