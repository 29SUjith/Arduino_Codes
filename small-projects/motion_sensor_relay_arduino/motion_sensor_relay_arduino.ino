#define PIR_PIN 2
#define RELAY_PIN 3

bool motionState = false;

void setup() {
  pinMode(PIR_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  Serial.begin(9600);
  delay(3000); // allow sensor to stabilize
}

void loop() {
  int motion = digitalRead(PIR_PIN);

  if (motion == HIGH && !motionState) {
    Serial.println("Motion detected!");
    digitalWrite(RELAY_PIN, LOW);
    motionState = true;
  } 
  else if (motion == LOW && motionState) {
    Serial.println("Motion ended!");
    digitalWrite(RELAY_PIN, HIGH);
    motionState = false;
  }

  delay(100);
}
