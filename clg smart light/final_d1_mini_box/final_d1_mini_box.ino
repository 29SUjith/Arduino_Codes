#define TRIG1 D5
#define ECHO1 D6
#define TRIG2 D1
#define ECHO2 D2
#define RELAY D7

const int threshold = 70;

unsigned long detectionTime1 = 0;
unsigned long detectionTime2 = 0;
int personCount = 0;
bool lightState = false;  // false = OFF, true = ON

void setup() {
  Serial.begin(115200);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH); // Light OFF initially

  Serial.println("People Counter - Standalone Mode");
}

void loop() {
  long dist1 = getDistance(TRIG1, ECHO1);
  long dist2 = getDistance(TRIG2, ECHO2);
  unsigned long now = millis();

  // Entry logic
  if (dist1 < threshold && (now - detectionTime1 > 1000)) {
    detectionTime1 = now;
    if (now - detectionTime2 < 1000) {
      personCount++;
      Serial.print("Person ENTERED. Count: ");
      Serial.println(personCount);
      turnLightOn();
    }
  }

  // Exit logic
  if (dist2 < threshold && (now - detectionTime2 > 1000)) {
    detectionTime2 = now;
    if (now - detectionTime1 < 1000) {
      if (personCount > 0) {
        personCount--;
        Serial.print("Person EXITED. Count: ");
        Serial.println(personCount);
      }
      if (personCount == 0) {
        turnLightOff();
      }
    }
  }

  delay(100);
}

long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return (duration == 0) ? 999 : distance;
}

void turnLightOn() {
  lightState = true;
  digitalWrite(RELAY, LOW); // Active LOW relay
}

void turnLightOff() {
  lightState = false;
  digitalWrite(RELAY, HIGH);
}