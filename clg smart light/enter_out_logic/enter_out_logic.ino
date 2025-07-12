#define TRIG1 D1
#define ECHO1 D2
#define TRIG2 D5
#define ECHO2 D6
#define RELAY D7
const int threshold = 50;
unsigned long detectionTime1 = 0;
unsigned long detectionTime2 = 0;
bool personInside = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, LOW); // Light OFF initially
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

void loop() {
  long dist1 = getDistance(TRIG1, ECHO1);
  long dist2 = getDistance(TRIG2, ECHO2);
  unsigned long now = millis();
  
  // Detection at sensor 1
  if (dist1 < threshold && (now - detectionTime1 > 1000)) {
    detectionTime1 = now;
    if (now - detectionTime2 < 1000) {
      Serial.println("Person ENTERED");
      personInside = true;
      digitalWrite(RELAY, HIGH); // Turn ON light
    }
  }
  
  // Detection at sensor 2
  if (dist2 < threshold && (now - detectionTime2 > 1000)) {
    detectionTime2 = now;
    if (now - detectionTime1 < 1000) {
      Serial.println("Person EXITED");
      personInside = false;
      digitalWrite(RELAY, LOW); // Turn OFF light
    }
  }
  
  delay(100);
}