#include <BluetoothSerial.h>

BluetoothSerial SerialBT;

#define M1_FORWARD 15
#define M1_BACKWARD 18
#define M2_FORWARD 5
#define M2_BACKWARD 19

int speed = 128;  // Default speed (0-255)

// Speed levels
const int LOW_SPEED = 85;
const int MEDIUM_SPEED = 170;
const int HIGH_SPEED = 255;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Car");  // Bluetooth name

  pinMode(M1_FORWARD, OUTPUT);
  pinMode(M1_BACKWARD, OUTPUT);
  pinMode(M2_FORWARD, OUTPUT);
  pinMode(M2_BACKWARD, OUTPUT);

  Serial.println("Bluetooth Car with 3-Speed Control Ready!");
}

// Movement functions
void moveForward() {
  analogWrite(M1_FORWARD, speed);
  analogWrite(M1_BACKWARD, 0);
  analogWrite(M2_FORWARD, speed);
  analogWrite(M2_BACKWARD, 0);
}

void moveBackward() {
  analogWrite(M1_FORWARD, 0);
  analogWrite(M1_BACKWARD, speed);
  analogWrite(M2_FORWARD, 0);
  analogWrite(M2_BACKWARD, speed);
}

void turnLeft() {
  analogWrite(M1_FORWARD, 0);
  analogWrite(M1_BACKWARD, speed);
  analogWrite(M2_FORWARD, speed);
  analogWrite(M2_BACKWARD, 0);
}

void turnRight() {
  analogWrite(M1_FORWARD, speed);
  analogWrite(M1_BACKWARD, 0);
  analogWrite(M2_FORWARD, 0);
  analogWrite(M2_BACKWARD, speed);
}

void stopCar() {
  analogWrite(M1_FORWARD, 0);
  analogWrite(M1_BACKWARD, 0);
  analogWrite(M2_FORWARD, 0);
  analogWrite(M2_BACKWARD, 0);
}

void loop() {
  if (SerialBT.available()) {
    char command = SerialBT.read();
    Serial.print("Command Received: ");
    Serial.println(command);

    // Movement commands
    if (command == 'F') moveForward();    // Forward
    if (command == 'B') moveBackward();   // Backward
    if (command == 'L') turnLeft();       // Left
    if (command == 'R') turnRight();      // Right
    if (command == 'S') stopCar();        // Stop

    // Speed control commands
    if (command == '1') {
      speed = LOW_SPEED;
      Serial.println("Speed: LOW");
    }
    if (command == '2') {
      speed = MEDIUM_SPEED;
      Serial.println("Speed: MEDIUM");
    }
    if (command == '3') {
      speed = HIGH_SPEED;
      Serial.println("Speed: HIGH");
    }
  }
}