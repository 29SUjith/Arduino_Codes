#include <Servo.h>

Servo ESC; 

void setup(){
  ESC.attach(10);  // Attach ESC signal to pin 10
  ESC.writeMicroseconds(1000);  // Minimum throttle to arm ESC
  delay(3000);  // Wait for ESC to initialize
}

void loop(){
  ESC.writeMicroseconds(1400);  // Medium speed (1000 = Off, 2000 = Full speed)
  delay(5000);  // Run for 5 seconds

  ESC.writeMicroseconds(1000);  // Stop the motor
  delay(5000);  // Wait before restarting
}
