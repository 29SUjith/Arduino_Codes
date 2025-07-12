#include <Servo.h>

Servo myServo;  // Create a servo object

void setup() {
    myServo.attach(9);  // Attach servo to pin 9

}

void loop() {
    // No continuous action needed, servo stays at 90 degrees
    myServo.write(90);  // Move servo to 90 degrees
}
