#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27
#define ARM_SERVO 15   // Servo for arm movement
#define WRIST_SERVO 4  // Servo for wrist rotation
#define GRIPPER_SERVO 18 // Servo for gripper

Servo armServo, wristServo, gripperServo;

unsigned long lastReceiveTime = 0;
const unsigned long timeoutInterval = 1000;
int x;

typedef struct {
    float pitch;
    float roll;
    bool gripper;
    bool mode;
} MotionData;

MotionData receivedData;

void OnDataRecv(const esp_now_recv_info_t *info, const uint8_t *incomingData, int len) {
    memcpy(&receivedData, incomingData, sizeof(receivedData));
    lastReceiveTime = millis();

    if (!receivedData.mode) {
        // --- Rover Control ---
        if (receivedData.pitch > 0.5) {  // Forward
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        } else if (receivedData.pitch < -0.5) {  // Backward
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
        } else if (receivedData.roll < -0.5) {  // Right
            digitalWrite(IN1, HIGH);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, HIGH);
        } else if (receivedData.roll > 0.5) {  // Left
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, HIGH);
            digitalWrite(IN3, HIGH);
            digitalWrite(IN4, LOW);
        } else {  // Stop
            digitalWrite(IN1, LOW);
            digitalWrite(IN2, LOW);
            digitalWrite(IN3, LOW);
            digitalWrite(IN4, LOW);
        }
    } else {
        // --- Arm Control ---
        int armAngle = map(receivedData.pitch * 100, -100, 100, 180, 0);
        int wristAngle = map(receivedData.roll * 100, -100, 100, 0, 180);
        
        armServo.write(armAngle);
        wristServo.write(wristAngle);
        gripperServo.write(x);
        
        Serial.print("Arm Angle: ");
        Serial.print(armAngle);
        Serial.print(" | Wrist Angle: ");
        Serial.println(wristAngle);

        // --- Gripper Control ---
        if (receivedData.gripper) {
            x=95;//close
            Serial.println("close");
        } else {
            x=40;// Open
            Serial.println("open");
        }
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    armServo.attach(ARM_SERVO);
    wristServo.attach(WRIST_SERVO);
    gripperServo.attach(GRIPPER_SERVO);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_recv_cb(OnDataRecv);
}

void loop() {
    if (millis() - lastReceiveTime > timeoutInterval) {
        digitalWrite(IN1, LOW);
        digitalWrite(IN2, LOW);
        digitalWrite(IN3, LOW);
        digitalWrite(IN4, LOW);
    }
}
