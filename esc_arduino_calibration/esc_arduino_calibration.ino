#include <Servo.h>

Servo ESC;

void setup() {
    ESC.attach(10);
    ESC.writeMicroseconds(2000);  // Max throttle (Enter calibration mode)
    delay(3000);
    ESC.writeMicroseconds(1000);  // Min throttle
    delay(3000);
}

void loop() {
    ESC.writeMicroseconds(1000);  // Keep ESC at minimum throttle
}
