#include <BluetoothSerial.h>

// Create BluetoothSerial object
BluetoothSerial SerialBT;

// Define motor control pins
#define IN1_FL 15
#define IN2_FL 4

#define IN1_FR 18
#define IN2_FR 5

#define IN1_BL 19
#define IN2_BL 21

#define IN1_BR 23
#define IN2_BR 22

int speed1 = 26;
int speed2 = 27;
int spa = 150;
int spb = 150;

void setup() {
  Serial.begin(115200);
  SerialBT.begin("ESP32_Mecanum_Forklift"); // Name your Bluetooth device
  Serial.println("Bluetooth device started, pair with 'ESP32_Mecanum'");

  // Set motor pins as outputs
  pinMode(IN1_FL, OUTPUT);
  pinMode(IN2_FL, OUTPUT);
  pinMode(IN1_FR, OUTPUT);
  pinMode(IN2_FR, OUTPUT);
  pinMode(IN1_BL, OUTPUT);
  pinMode(IN2_BL, OUTPUT);
  pinMode(IN1_BR, OUTPUT);
  pinMode(IN2_BR, OUTPUT);
  pinMode(speed1, OUTPUT);
  pinMode(speed2, OUTPUT);
}

void loop() {
  if (SerialBT.available()) {
    String command = SerialBT.readStringUntil('\n');
    Serial.println("Received command: " + command);
    parseCommand(command);
  }
}

void controlMotor(int in1, int in2, bool direction) {
  digitalWrite(in1, direction ? HIGH : LOW);
  digitalWrite(in2, direction ? LOW : HIGH);
}

void mecanumControl(bool frontLeftDir, bool frontRightDir, bool backLeftDir, bool backRightDir) {
  controlMotor(IN1_FL, IN2_FL, frontLeftDir);
  controlMotor(IN1_FR, IN2_FR, frontRightDir);
  controlMotor(IN1_BL, IN2_BL, backLeftDir);
  controlMotor(IN1_BR, IN2_BR, backRightDir);
}

void back() {
  digitalWrite(IN1_FL, HIGH); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, HIGH); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, HIGH); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void front() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void left() {
  digitalWrite(IN1_FL, HIGH); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, HIGH); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, 200); analogWrite(speed2, 200);
}

void right() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, HIGH); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1, 200); analogWrite(speed2, 200);
}

void stop() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void three_one_five() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, HIGH); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, HIGH); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void two_two_five() {
  digitalWrite(IN1_FL, HIGH); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, HIGH); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void one_three_five() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, LOW);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, HIGH);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, HIGH);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, LOW);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void four_five() {
  digitalWrite(IN1_FL, LOW); digitalWrite(IN2_FL, HIGH);
  digitalWrite(IN1_FR, LOW); digitalWrite(IN2_FR, LOW);
  digitalWrite(IN1_BL, LOW); digitalWrite(IN2_BL, LOW);
  digitalWrite(IN1_BR, LOW); digitalWrite(IN2_BR, HIGH);
  analogWrite(speed1, spa); analogWrite(speed2, spb);
}

void parseCommand(String cmd) {
  cmd.trim();

  if (cmd == "F") {
    front();
  } else if (cmd == "B") {
    back();
  } else if (cmd == "L") {
    left();
  } else if (cmd == "R") {
    right();
  } else if (cmd == "CW") {
    mecanumControl(true, false, true, false);
  } else if (cmd == "CCW") {
    mecanumControl(false, true, false, true);
  } else if (cmd == "45") {
    four_five();
  } else if (cmd == "135") {
    one_three_five();
  } else if (cmd == "225") {
    two_two_five();
  } else if (cmd == "315") {
    three_one_five();
  } else if (cmd == "S") {
    stop();
  } else {
    Serial.println("Unknown command: " + cmd);
  }
}
