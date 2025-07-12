 #include <HardwareSerial.h>


#define TOUCH_PIN 4
#define PHONE_NUMBER "+917827191427"   // Replace with your number

HardwareSerial simSerial(2); // UART2 for EC200U: RX2 = 16, TX2 = 17

unsigned long lastPress = 0;
int pressCount = 0;

void setup() {
  Serial.begin(115200);
  pinMode(TOUCH_PIN, INPUT);
  
  simSerial.begin(115200, SERIAL_8N1, 16, 17); // EC200U on Serial2

  Serial.println("System ready...");
}

void loop() {
  if (digitalRead(TOUCH_PIN) == HIGH) {
    unsigned long now = millis();
    if (now - lastPress < 1000) {
      pressCount++;
    } else {
      pressCount = 1;
    }
    lastPress = now;
    delay(300);  // debounce delay

    if (pressCount == 2) {
      sendSMS("Touch detected twice! Sending Emergency SMS");
      makeCall();
      pressCount = 0;
    }
  }
}

void sendSMS(String message) {
  simSerial.println("AT+CMGF=1"); delay(1000);
  simSerial.print("AT+CMGS=\""); simSerial.print(PHONE_NUMBER); simSerial.println("\""); delay(1000);
  simSerial.print(message); delay(100);
  simSerial.write(26); // CTRL+Z to send
  Serial.println("SMS Sent!");
}

void makeCall() {
  simSerial.println("AT"); // Basic check
  delay(500);

  simSerial.println("AT+CPAS"); // Check if device is ready
  delay(500);

  simSerial.println("AT+CFUN=1"); // Full functionality
  delay(500);

  String dialCommand = "ATD" + String(PHONE_NUMBER) + ";";
  simSerial.println(dialCommand); // Dial the number
  Serial.println("Calling...");

  // Optional: Print modem response for debugging
  unsigned long timeout = millis() + 10000;
  while (millis() < timeout) {
    if (simSerial.available()) {
      String response = simSerial.readStringUntil('\n');
      Serial.println(">> " + response);
    }
  }

  delay(20000); // Let it ring for 15 seconds

  simSerial.println("ATH"); // Hang up
  Serial.println("Call ended.");
}

