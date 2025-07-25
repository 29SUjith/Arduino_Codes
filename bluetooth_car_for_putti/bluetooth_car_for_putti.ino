#include <esp_now.h>
#include <WiFi.h>

#define IN1 18
#define IN2 19
#define IN3 21
#define IN4 22
#define ENA 25
#define ENB 26

int baseSpeed = 250;
int currentSpeedA = 250;
int currentSpeedB = 250;

// Structure to receive joystick data
typedef struct struct_message {
  int xPos;
  int yPos;
} struct_message;

struct_message joystickData;

void setup() {
  Serial.begin(115200);
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(onDataReceive);
  
  pinMode(IN1, OUTPUT); pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT); pinMode(ENB, OUTPUT);

  // Initialize motors to stop
  stopMotors();
  
  Serial.println("ESP-NOW Car Receiver Ready");
}

void loop() {
  // Main loop is empty as we handle everything in the callback
  delay(10);
}

// Callback function - compatible with newer ESP32 core versions
void onDataReceive(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&joystickData, incomingData, sizeof(joystickData));
  
  Serial.print("X: "); Serial.print(joystickData.xPos);
  Serial.print(" Y: "); Serial.println(joystickData.yPos);
  
  processJoystickInput(joystickData.xPos, joystickData.yPos);
}

void processJoystickInput(int xPos, int yPos) {
  // Convert joystick values (-512 to 512) to motor control
  // Dead zone to prevent drift
  int deadZone = 50;
  
  if (abs(xPos) < deadZone && abs(yPos) < deadZone) {
    stopMotors();
    return;
  }
  
  // Calculate base speed from Y axis (forward/backward)
  int forwardSpeed = map(abs(yPos), deadZone, 512, 0, 255);
  forwardSpeed = constrain(forwardSpeed, 0, 255);
  
  // Calculate turn adjustment from X axis
  int turnAdjustment = map(abs(xPos), deadZone, 512, 0, 100);
  turnAdjustment = constrain(turnAdjustment, 0, 100);
  
  // Determine direction and apply differential steering
  if (yPos > deadZone) {
    // Moving forward
    if (xPos > deadZone) {
      // Forward right
      currentSpeedA = forwardSpeed;
      currentSpeedB = forwardSpeed - (forwardSpeed * turnAdjustment / 100);
    } else if (xPos < -deadZone) {
      // Forward left
      currentSpeedA = forwardSpeed - (forwardSpeed * turnAdjustment / 100);
      currentSpeedB = forwardSpeed;
    } else {
      // Straight forward
      currentSpeedA = forwardSpeed;
      currentSpeedB = forwardSpeed;
    }
    moveForwardDifferential();
  } else if (yPos < -deadZone) {
    // Moving backward
    if (xPos > deadZone) {
      // Backward right
      currentSpeedA = forwardSpeed;
      currentSpeedB = forwardSpeed - (forwardSpeed * turnAdjustment / 100);
    } else if (xPos < -deadZone) {
      // Backward left
      currentSpeedA = forwardSpeed - (forwardSpeed * turnAdjustment / 100);
      currentSpeedB = forwardSpeed;
    } else {
      // Straight backward
      currentSpeedA = forwardSpeed;
      currentSpeedB = forwardSpeed;
    }
    moveBackwardDifferential();
  } else {
    // Only turning (Y near zero)
    if (xPos > deadZone) {
      currentSpeedA = turnAdjustment * 2;
      currentSpeedB = turnAdjustment * 2;
      turnRight();
    } else if (xPos < -deadZone) {
      currentSpeedA = turnAdjustment * 2;
      currentSpeedB = turnAdjustment * 2;
      turnLeft();
    }
  }
}

void moveForwardDifferential() {
  analogWrite(ENA, currentSpeedA);
  analogWrite(ENB, currentSpeedB);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackwardDifferential() {
  analogWrite(ENA, currentSpeedA);
  analogWrite(ENB, currentSpeedB);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void moveForward() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  analogWrite(ENA, baseSpeed);
  analogWrite(ENB, baseSpeed);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  analogWrite(ENA, currentSpeedA);
  analogWrite(ENB, currentSpeedB);
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void turnRight() {
  analogWrite(ENA, currentSpeedA);
  analogWrite(ENB, currentSpeedB);
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}