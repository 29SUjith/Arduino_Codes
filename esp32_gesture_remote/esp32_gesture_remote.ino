#include <Wire.h>
#include <esp_now.h>
#include <WiFi.h>

#define MPU6050_ADDR 0x68  // MPU6050 I2C address
#define TOGGLE_BUTTON 18   // Button to toggle control mode
#define GRIPPER_BUTTON 4   // Button to control gripper

bool controlMode = false;  // false = Rover, true = Robotic Arm

typedef struct {
    float pitch;  // Forward/Backward for rover or arm
    float roll;   // Right/Left for rover or arm
    bool gripper; // Gripper state (true = close, false = open)
    bool mode;    // Control mode (false = Rover, true = Arm)
} MotionData;

MotionData motionData;

uint8_t roverMac[] = {0x78, 0x42, 0x1c, 0x6c, 0xbd, 0x94}; //ESP32 MAC 78:42:1c:6c:bd:94

void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
    Serial.print("Send Status: ");
    Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void setup() {
    Serial.begin(115200);
    Wire.begin();

    pinMode(TOGGLE_BUTTON, INPUT_PULLUP);
    pinMode(GRIPPER_BUTTON, INPUT_PULLUP);

    WiFi.mode(WIFI_STA);
    if (esp_now_init() != ESP_OK) {
        Serial.println("ESP-NOW Init Failed");
        return;
    }

    esp_now_register_send_cb(OnDataSent);

    esp_now_peer_info_t peerInfo;
    memcpy(peerInfo.peer_addr, roverMac, 6);
    peerInfo.channel = 0;
    peerInfo.encrypt = false;
    
    if (esp_now_add_peer(&peerInfo) != ESP_OK) {
        Serial.println("Failed to add peer");
        return;
    }

    // Initialize MPU6050
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x6B);
    Wire.write(0);
    Wire.endTransmission();
}

void loop() {
    // Toggle control mode on button press (Debounce)
    static bool lastButtonState = HIGH;
    bool buttonState = digitalRead(TOGGLE_BUTTON);
    if (buttonState == LOW && lastButtonState == HIGH) {
        controlMode = !controlMode;
        Serial.print("Mode Changed: ");
        Serial.println(controlMode ? "Arm Control" : "Rover Control");
        delay(300); // Debounce
    }
    lastButtonState = buttonState;

    // Read MPU6050 data
    Wire.beginTransmission(MPU6050_ADDR);
    Wire.write(0x3B);
    Wire.endTransmission(false);
    Wire.requestFrom(MPU6050_ADDR, 6, true);

    int16_t accX = (Wire.read() << 8 | Wire.read());
    int16_t accY = (Wire.read() << 8 | Wire.read());

    motionData.pitch = accX / 16384.0;
    motionData.roll = accY / 16384.0;
    motionData.mode = controlMode;
    motionData.gripper = digitalRead(GRIPPER_BUTTON) == LOW; // If pressed, close gripper

    esp_err_t result = esp_now_send(roverMac, (uint8_t *)&motionData, sizeof(motionData));
    if (result != ESP_OK) {
        Serial.println("Failed to send data");
    }

    delay(100); 
}
