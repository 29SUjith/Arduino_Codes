// Motor direction pins
#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

// Speed control pin
#define SPEED_PIN 15

// Pump pin
#define PUMP_PIN 23
 
// Ultrasonic sensors
#define TRIG_FRONT 4
#define ECHO_FRONT 5
#define TRIG_LEFT 19
#define ECHO_LEFT 18
#define TRIG_RIGHT 21
#define ECHO_RIGHT 22

int currentSpeed = 150;

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(SPEED_PIN, OUTPUT);
  analogWrite(SPEED_PIN, currentSpeed);

  pinMode(PUMP_PIN, OUTPUT);

  pinMode(TRIG_FRONT, OUTPUT); pinMode(ECHO_FRONT, INPUT);
  pinMode(TRIG_LEFT, OUTPUT);  pinMode(ECHO_LEFT, INPUT);
  pinMode(TRIG_RIGHT, OUTPUT); pinMode(ECHO_RIGHT, INPUT);

  Serial.println("Bot ready with analogWrite speed control.");
}

void loop() {
  if (Serial.available()) {
    char cmd = Serial.read();

    if (cmd == 'F') {
      moveForward();
    } else if (cmd == 'B') {
      moveBackward();
    } else if (cmd == 'L') {
      turnLeft();
    } else if (cmd == 'R') {
      turnRight();
    } else if (cmd == 'S') {
      stopMotors();
    } else if (cmd == 'W') {
      digitalWrite(PUMP_PIN, HIGH);
    } else if (cmd == 'w') {
      digitalWrite(PUMP_PIN, LOW);
    }

    // Speed control
    else if (cmd == '1') {
      currentSpeed = 100; Serial.println("Speed: Low");
    } else if (cmd == '2') {
      currentSpeed = 150; Serial.println("Speed: Medium");
    } else if (cmd == '3') {
      currentSpeed = 200; Serial.println("Speed: Fast");
    } else if (cmd == '4') {
      currentSpeed = 255; Serial.println("Speed: Max");
    }

    analogWrite(SPEED_PIN, currentSpeed); // Apply updated speed
  }

  // Print ultrasonic sensor readings
  Serial.print("Front: "); Serial.print(getDistance(TRIG_FRONT, ECHO_FRONT));
  Serial.print(" cm, Left: "); Serial.print(getDistance(TRIG_LEFT, ECHO_LEFT));
  Serial.print(" cm, Right: "); Serial.println(getDistance(TRIG_RIGHT, ECHO_RIGHT));

  delay(500);
}

// Movement functions
void moveForward() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void moveBackward() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void turnLeft() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void turnRight() {
  digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void stopMotors() {
  digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// Ultrasonic distance function
long getDistance(int trig, int echo) {
  digitalWrite(trig, LOW); delayMicroseconds(2);
  digitalWrite(trig, HIGH); delayMicroseconds(10);
  digitalWrite(trig, LOW);
  long duration = pulseIn(echo, HIGH, 20000);
  return duration == 0 ? -1 : (duration * 0.034 / 2);
}
