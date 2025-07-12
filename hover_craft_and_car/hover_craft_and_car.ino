#include <Servo.h>

#define RAIN_SENSOR_PIN A0  // Define the rain sensor pin
#define RAIN_THRESHOLD 400  // Define a threshold value for rain detection

Servo ESC1;   // First ESC (Up)
Servo ESC2;   // Second ESC (Front)
Servo Servo3; // Third Servo (for additional movement)

bool upMotorLocked = false;  // Flag to keep the up motor running

void setup() {
  ESC1.attach(9);    // First ESC on pin 9 (Up)
  ESC2.attach(10);   // Second ESC on pin 10 (Front)
  Servo3.attach(11); // Third Servo on pin 11

  pinMode(RAIN_SENSOR_PIN, INPUT);

  ESC1.writeMicroseconds(1000);  // Arm first ESC
  ESC2.writeMicroseconds(1000);  // Arm second ESC
  Servo3.write(117);  // Set third servo to middle position

  delay(3000);  // Wait for ESCs to initialize

  Serial.begin(9600);  // Start Bluetooth communication via Hardware Serial
}

void loop() {
  int rainValue = analogRead(RAIN_SENSOR_PIN);
  Serial.print("Rain Sensor Value: ");
  Serial.println(rainValue);

  if (rainValue > RAIN_THRESHOLD) {  // If rain is detected
    ESC1.writeMicroseconds(1400);  // Move up motor
    upMotorLocked = true;  // Lock the up motor on
  }

  if (Serial.available()) {  // Check if Bluetooth sent data
    char command = Serial.read(); // Read command
    Serial.println(command);

    // Control ESC2 (Front motor)
    if (command == 'F') {  
      ESC2.writeMicroseconds(1800);  // Move forward
    } 
    else if (command == 'B') {  
      ESC2.writeMicroseconds(2000);  // Move forward faster
    } 
    else if (command == 'C') {  
      ESC2.writeMicroseconds(1400);  // Move forward faster
    } 
    else if (command == 'S') {  
      ESC2.writeMicroseconds(1000);  // Stop front motor
    }

    // Control ESC1 (Up motor) only if not locked by rain sensor
    if (!upMotorLocked) {
      if (command == 'U') {  
        ESC1.writeMicroseconds(1400);  // Move up
      } 
      else if (command == 'V') {  
        ESC1.writeMicroseconds(1600);  // Move up faster
      }
      else if (command == 'W') {  
        ESC1.writeMicroseconds(1800);  // Move up faster
      }
      else if (command == 'Z') {  
        ESC1.writeMicroseconds(2000);  // Move up faster
      }
    }
    
    if (command == 'D') {  
      ESC1.writeMicroseconds(1000);  // Stop up motor
      upMotorLocked = false;  // Unlock the up motor
    }

    // Control Servo3
    else if (command == 'L') {  
      Servo3.write(45);  // Move servo to left position
    } 
    else if (command == 'R') {  
      Servo3.write(165);  // Move servo to right position
    } 
    else if (command == 'M') {  
      Servo3.write(117);  // Reset servo to middle position
    }
  }
}
