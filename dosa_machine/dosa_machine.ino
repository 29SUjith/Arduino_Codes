#include <Servo.h>

const byte BTN_PIN = 2;            // Push-button to GND

Servo servo1, servo2, servo3;

// ---------- Initial positions ----------
const int INIT_S1 = 90;
const int INIT_S2 = 0;
const int INIT_S3 = 0;

// ---------- Sequence targets ----------
const int SEQ_S3 = 150;             // first
const int SEQ_S1 = 180;            // second
const int SEQ_S2 = 125;             // third

bool lastBtnState      = HIGH;     // INPUT_PULLUP: HIGH = released
unsigned long debounce = 0;
const unsigned long DEBOUNCE_MS = 25;

void go(Servo &s, int angle, unsigned long wait = 1000) {
  s.write(angle);
  delay(wait);                     // simple blocking wait
}

void runSequence() {
  // ----- Step 1: Servo-3 out & back -----
  go(servo3, SEQ_S3);        // 0° → 90°
  go(servo3, INIT_S3);       // 90° → 0°

  // ----- Step 2: Servo-1 and Servo-2 -----
  go(servo1, SEQ_S1);        // 0° → 180°
  go(servo2, SEQ_S2);        // 0° → 90°

  delay(500);               // hold the pose (optional)

  // ----- Return to start (S2 → S1) -----
  go(servo2, INIT_S2);       // 90° → 0°
  go(servo1, INIT_S1);       // 180° → 90°
  // Servo-3 is already at INIT_S3 (0°)
}


void setup() {
  Serial.begin(9600);

  servo1.attach(9);
  servo2.attach(10);
  servo3.attach(11);

  // park at initial angles
  servo1.write(INIT_S1);
  servo2.write(INIT_S2);
  servo3.write(INIT_S3);

  pinMode(BTN_PIN, INPUT_PULLUP);

  Serial.println(F("Serial: <servo#> <0-180>.  Button = autoplay sequence."));
}

void loop() {
  /* ---------- Serial command control ---------- */
  if (Serial.available()) {
    int num = Serial.parseInt();
    int ang = Serial.parseInt();
    if (num >= 1 && num <= 3 && ang >= 0 && ang <= 180) {
      switch (num) {
        case 1: servo1.write(ang); break;
        case 2: servo2.write(ang); break;
        case 3: servo3.write(ang); break;
      }
      Serial.print(F("Servo ")); Serial.print(num);
      Serial.print(F(" -> "));    Serial.println(ang);
    } else {
      Serial.println(F("Use: 1-3  0-180"));
    }
    while (Serial.available()) Serial.read();   // clear buffer
  }

  /* ---------- Push-button handling ---------- */
  bool reading = digitalRead(BTN_PIN);
  if (reading != lastBtnState) {
    debounce = millis();
    lastBtnState = reading;
  }
  if ((millis() - debounce) > DEBOUNCE_MS && reading == LOW) {
    runSequence();                             // fire once
    while (digitalRead(BTN_PIN) == LOW) delay(10); // wait for release
    lastBtnState = HIGH;
  }
}
