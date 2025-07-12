// Pin definitions for L298N motor driver
#define IN1 5  // Motor 1 forward
#define IN2 6  // Motor 1 backward
#define IN3 9  // Motor 2 forward
#define IN4 10 // Motor 2 backward
int SP1 = 120; // Motor speed

// Bluetooth module connected to Arduino's RX and TX pins
#define BAUD_RATE 9600

// Ultrasonic sensor pins
#define TRIG 7
#define ECHO 8

char val; // Variable to store Bluetooth command

void setup() {
  // Initialize motor pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(11, OUTPUT);

  // Initialize ultrasonic sensor pins
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);

  // Turn off motors initially
  stopMotors();

  // Start serial communication
  Serial.begin(BAUD_RATE);
}

void loop() {
  // Check for Bluetooth commands
  if (Serial.available() > 0) {
    val = Serial.read();
    Serial.println(val);
  }
  analogWrite(3, SP1);
  analogWrite(11, SP1);

  // Get distance from ultrasonic sensor
  float distance = getDistance();
  Serial.println(distance);

  // Movement logic
  if (distance > 50) { // Safe distance
    if (val == 'F') {
      moveForward();
    } else if (val == 'L') {
      turnLeft();
    } else if (val == 'R') {
      turnRight();
    } else if (val == 'S') {
      stopMotors();
    }
  } else { // Obstacle detected
    if (val == 'B') {
      moveBackward();
      delay(500);
    } else if (val == 'S') {
      stopMotors();
    } else {
      stopMotors(); // Stop if no valid command for close obstacle
    }
  }

  // Adjust speed
  if (val == '1') {
    SP1 = 150;
  } else if (val == '2') {
    SP1 = 190;
  } else if (val == '3') {
    SP1 = 250;
  }

  delay(100);
}


// Functions to control motors
void moveForward() {
   // Move only if distance is greater than 50 cm
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    Serial.println("Moving forward");
  
}

void moveBackward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Moving backward");
}

void turnLeft() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  Serial.println("Turning left");
}

void turnRight() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  Serial.println("Turning right");
}

void stopMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  Serial.println("Motors stopped");
}

// Function to get distance from ultrasonic sensor
float getDistance() {
  // Send a 10us pulse to trigger pin
  digitalWrite(TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG, LOW);

  // Measure the duration of the pulse on the echo pin
  long duration = pulseIn(ECHO, HIGH);

  // Calculate distance in cm
  float distance = (duration * 0.034) / 2;

  return distance;
}
