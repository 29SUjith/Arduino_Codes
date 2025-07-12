#include <Servo.h>

Servo ESC1;   // First ESC (Up)
Servo ESC2;   // Second ESC (Front)
Servo Servo3; // Third Servo (for additional movement)

void setup() {
  ESC1.attach(9);    // First ESC on pin 9 (Up)
  ESC2.attach(10);   // Second ESC on pin 10 (Front)
  Servo3.attach(11); // Third Servo on pin 11

  ESC1.writeMicroseconds(1000);  // Arm first ESC
  ESC2.writeMicroseconds(1000);  // Arm second ESC
  Servo3.write(90);  // Set third servo to middle position

  delay(3000);  // Wait for ESCs to initialize

  Serial.begin(9600);  // Start Bluetooth communication via Hardware Serial
}

void loop() {
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
    else if (command == 'S') {  
      ESC2.writeMicroseconds(1000);  // Stop front motor
    }

    // Control ESC1 (Up motor)
    else if (command == 'U') {  
      ESC1.writeMicroseconds(1400);  // Move up
    } 
    else if (command == 'V') {  
      ESC1.writeMicroseconds(1600);  // Move up faster
    } 
    else if (command == 'D') {  
      ESC1.writeMicroseconds(1000);  // Stop up motor
    }

    // Control Servo3
    else if (command == 'L') {  
      Servo3.write(45);  // Move servo to left position
    } 
    else if (command == 'R') {  
      Servo3.write(135);  // Move servo to right position
    } 
    else if (command == 'M') {  
      Servo3.write(90);  // Reset servo to middle position
    }
  }
}
