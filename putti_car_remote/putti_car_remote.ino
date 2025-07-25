#include <esp_now.h>
#include <WiFi.h>

// Button pins
#define BUTTON_UP    5  // Forward
#define BUTTON_DOWN  18  // Backward
#define BUTTON_LEFT  19  // Left
#define BUTTON_RIGHT 21  // Right

// Optional additional buttons
#define BUTTON_EXTRA1 27  // Extra button 1 (horn, lights, etc.)
#define BUTTON_EXTRA2 26  // Extra button 2

// Button debounce settings
#define DEBOUNCE_DELAY 50  // Milliseconds
#define BUTTON_PRESSED LOW  // Change to HIGH if using pull-down resistors

// Your car's MAC address
uint8_t receiverAddress[] = {0x78, 0x42, 0x1C, 0x6C, 0xDF, 0xC8};

// Structure to send control data
typedef struct struct_message {
  int xPos;  // -512 (left) to 512 (right)
  int yPos;  // -512 (backward) to 512 (forward)
  bool extra1; // Extra button 1 state
  bool extra2; // Extra button 2 state
} struct_message;

struct_message controlData;

// Button state variables
struct ButtonState {
  bool currentState;
  bool lastState;
  unsigned long lastDebounceTime;
  bool pressed;
};

ButtonState buttons[6]; // 4 directional + 2 extra buttons
const int buttonPins[6] = {BUTTON_UP, BUTTON_DOWN, BUTTON_LEFT, BUTTON_RIGHT, BUTTON_EXTRA1, BUTTON_EXTRA2};

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  Serial.println("Starting ESP-NOW 4-Button Remote Controller...");
  
  // Print MAC address
  Serial.print("Remote MAC: ");
  Serial.println(WiFi.macAddress());
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  delay(100);
  
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  Serial.println("ESP-NOW initialized successfully");
  
  // Register for a callback function that will be called when data is sent
  esp_now_register_send_cb(onDataSent);
  
  // Register peer
  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  // Add peer
  esp_err_t addPeerResult = esp_now_add_peer(&peerInfo);
  if (addPeerResult != ESP_OK) {
    Serial.print("Failed to add peer: ");
    Serial.println(addPeerResult);
    return;
  }
  Serial.println("Peer added successfully");
  
  // Setup button pins with internal pull-up resistors
  for (int i = 0; i < 6; i++) {
    pinMode(buttonPins[i], INPUT_PULLUP);
    buttons[i].currentState = HIGH;
    buttons[i].lastState = HIGH;
    buttons[i].lastDebounceTime = 0;
    buttons[i].pressed = false;
  }
  
  // Initialize control data
  controlData.xPos = 0;
  controlData.yPos = 0;
  controlData.extra1 = false;
  controlData.extra2 = false;
  
  Serial.println("ESP-NOW 4-Button Remote Controller Ready");
  Serial.println("Button mapping:");
  Serial.println("- Pin 32: UP (Forward)");
  Serial.println("- Pin 33: DOWN (Backward)");
  Serial.println("- Pin 34: LEFT");
  Serial.println("- Pin 35: RIGHT");
  Serial.println("- Pin 27: EXTRA1");
  Serial.println("- Pin 26: EXTRA2");
  
  Serial.print("Target MAC: ");
  for (int i = 0; i < 6; i++) {
    Serial.printf("%02X", receiverAddress[i]);
    if (i < 5) Serial.print(":");
  }
  Serial.println();
}

void loop() {
  // Read and debounce all buttons
  readButtons();
  
  // Reset control values
  controlData.xPos = 0;
  controlData.yPos = 0;
  
  // Check directional buttons and set control values
  if (buttons[0].pressed) { // UP button
    controlData.yPos = 512;  // Forward
  }
  if (buttons[1].pressed) { // DOWN button
    controlData.yPos = -512; // Backward
  }
  if (buttons[2].pressed) { // LEFT button
    controlData.xPos = -512; // Left
  }
  if (buttons[3].pressed) { // RIGHT button
    controlData.xPos = 512;  // Right
  }
  
  // Handle diagonal movement (two buttons pressed simultaneously)
  if (buttons[0].pressed && buttons[2].pressed) { // UP + LEFT
    controlData.yPos = 362;  // Forward (512 * 0.707 for 45-degree angle)
    controlData.xPos = -362; // Left
  }
  if (buttons[0].pressed && buttons[3].pressed) { // UP + RIGHT
    controlData.yPos = 362;  // Forward
    controlData.xPos = 362;  // Right
  }
  if (buttons[1].pressed && buttons[2].pressed) { // DOWN + LEFT
    controlData.yPos = -362; // Backward
    controlData.xPos = -362; // Left
  }
  if (buttons[1].pressed && buttons[3].pressed) { // DOWN + RIGHT
    controlData.yPos = -362; // Backward
    controlData.xPos = 362;  // Right
  }
  
  // Handle extra buttons
  controlData.extra1 = buttons[4].pressed;
  controlData.extra2 = buttons[5].pressed;
  
  // Send data via ESP-NOW
  esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &controlData, sizeof(controlData));
  
  // Debug output
  Serial.print("Control - X: "); Serial.print(controlData.xPos);
  Serial.print(" Y: "); Serial.print(controlData.yPos);
  Serial.print(" Extra1: "); Serial.print(controlData.extra1);
  Serial.print(" Extra2: "); Serial.print(controlData.extra2);
  
  if (result == ESP_OK) {
    Serial.println(" - SUCCESS");
  } else {
    Serial.print(" - ERROR: ");
    switch(result) {
      case ESP_ERR_ESPNOW_NOT_INIT:
        Serial.println("ESP-NOW not initialized");
        break;
      case ESP_ERR_ESPNOW_ARG:
        Serial.println("Invalid argument");
        break;
      case ESP_ERR_ESPNOW_INTERNAL:
        Serial.println("Internal error");
        break;
      case ESP_ERR_ESPNOW_NO_MEM:
        Serial.println("Out of memory");
        break;
      case ESP_ERR_ESPNOW_NOT_FOUND:
        Serial.println("Peer not found");
        break;
      case ESP_ERR_ESPNOW_IF:
        Serial.println("Interface error");
        break;
      default:
        Serial.print("Unknown error: ");
        Serial.println(result);
        break;
    }
  }
  
  delay(50); // Send updates every 50ms
}

// Read and debounce all buttons
void readButtons() {
  unsigned long currentTime = millis();
  
  for (int i = 0; i < 6; i++) {
    // Read the button state
    bool reading = digitalRead(buttonPins[i]);
    
    // Check if button state has changed
    if (reading != buttons[i].lastState) {
      buttons[i].lastDebounceTime = currentTime;
    }
    
    // If enough time has passed, accept the reading
    if ((currentTime - buttons[i].lastDebounceTime) > DEBOUNCE_DELAY) {
      // If the button state has changed
      if (reading != buttons[i].currentState) {
        buttons[i].currentState = reading;
        
        // Update pressed state (button is pressed when reading is LOW due to pull-up)
        buttons[i].pressed = (buttons[i].currentState == BUTTON_PRESSED);
      }
    }
    
    buttons[i].lastState = reading;
  }
}

// Print button states for debugging
void printButtonStates() {
  Serial.print("Buttons: ");
  const char* buttonNames[] = {"UP", "DOWN", "LEFT", "RIGHT", "EXT1", "EXT2"};
  
  for (int i = 0; i < 6; i++) {
    if (buttons[i].pressed) {
      Serial.print(buttonNames[i]);
      Serial.print(" ");
    }
  }
  Serial.println();
}

// Callback when data is sent
void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  if (status != ESP_NOW_SEND_SUCCESS) {
    Serial.println("Delivery failed");
  }
}

// Helper function to get MAC address
void printMacAddress() {
  Serial.print("MAC Address: ");
  Serial.println(WiFi.macAddress());
}