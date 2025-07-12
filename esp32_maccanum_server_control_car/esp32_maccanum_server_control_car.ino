#include <WiFi.h>
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "Project";
const char* password = "12345678";

// Define motor control pins
#define IN1_FL 4 // Front-left motor direction 1
#define IN2_FL 5 // Front-left motor direction 2

#define IN1_FR 18 // Front-right motor direction 1
#define IN2_FR 19 // Front-right motor direction 2

#define IN1_BL 13 // Back-left motor direction 1
#define IN2_BL 12 // Back-left motor direction 2

#define IN1_BR 14 // Back-right motor direction 1
#define IN2_BR 27 // Back-right motor direction 2
int speed1=15;
int speed2=23;
int spa=150;
int spb=150;

// Create an AsyncWebServer object on port 80
AsyncWebServer server(80);

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Set motor pins as outputs
  
  pinMode(IN1_FL, OUTPUT);
  pinMode(IN2_FL, OUTPUT);

  pinMode(IN1_FR, OUTPUT);
  pinMode(IN2_FR, OUTPUT);

  pinMode(IN1_BL, OUTPUT);
  pinMode(IN2_BL, OUTPUT);

  pinMode(IN1_BR, OUTPUT);
  pinMode(IN2_BR, OUTPUT);

  pinMode(speed1,OUTPUT);
  pinMode(speed2,OUTPUT);
  // Handle POST request to "/command"
  server.on("/command", HTTP_POST, [](AsyncWebServerRequest *request){
    String command = request->getParam("command", true)->value();
    Serial.println("Received command: " + command);
    parseCommand(command);
    request->send(200, "text/plain", "Command received: " + command);
  });

  // Start the server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Nothing to do in loop as it's asynchronous
}


void controlMotor(int in1, int in2, bool direction) {
  if (direction) {
    digitalWrite(in1, HIGH);
    digitalWrite(in2, LOW);
  } else {
    digitalWrite(in1, LOW);
    digitalWrite(in2, HIGH);
  }
}
void mecanumControl(bool frontLeftDir, bool frontRightDir, bool backLeftDir, bool backRightDir) {
  controlMotor(IN1_FL, IN2_FL, frontLeftDir);
  controlMotor(IN1_FR, IN2_FR, frontRightDir);
  controlMotor(IN1_BL, IN2_BL, backLeftDir);
  controlMotor(IN1_BR, IN2_BR, backRightDir);
}
void back(){
  digitalWrite(IN1_FL, HIGH);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, HIGH);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, HIGH);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void front(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void left(){
  digitalWrite(IN1_FL, HIGH);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, HIGH);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,200);
  analogWrite(speed2,200);
}
void right(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, HIGH);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1,200);
  analogWrite(speed2,200);
}

void stop(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void three_one_five(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, HIGH);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void two_two_five(){
  digitalWrite(IN1_FL, HIGH);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, HIGH);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void one_three_five(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, LOW);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}

void four_five(){
  digitalWrite(IN1_FL, LOW);
  digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, LOW);
  digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW);
  digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW);
  digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1,spa);
  analogWrite(speed2,spb);
}
void parseCommand(String cmd) {
  
  cmd.trim();
  
  if (cmd == "F") {
    Serial.println("Moving forward");
    front();
  } else if (cmd == "B") {
    Serial.println("Moving backward");
    back();
  } else if (cmd == "L") {
    Serial.println("Strafing left");
    left();
  } else if (cmd == "R") {
    Serial.println("Strafing right");
    right();
  } else if (cmd == "CW") {
    Serial.println("Rotating clockwise");
    mecanumControl(true, false, true, false);
  } else if (cmd == "CCW") {
    Serial.println("Rotating counterclockwise");
    mecanumControl(false, true, false, true);
  } else if (cmd == "45") {
    Serial.println("Moving diagonally forward-left");
    four_five();
  } else if (cmd == "135") {
    Serial.println("Moving diagonally forward-right");
    one_three_five();
  } else if (cmd == "225") {
    Serial.println("Turning left 90 degrees");
    two_two_five();
  } else if (cmd == "315") {
    Serial.println("Turning right 90 degrees");
    three_one_five();
  } else if (cmd == "S") {
    Serial.println("Stopping");
    stop();  
  } else {
    Serial.println("Unknown command");
  }
}