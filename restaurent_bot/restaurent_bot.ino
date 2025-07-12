int m1a = 2;
int m1b = 3;
int m2a = 4;
int m2b = 5;
int ir1; // right
int ir2;
int ir3; 
int ir4; // left
int ir5;
int s1;
int s2;
int s3;
int sp1 = 9;
int sp2 = 10;
int speed = 180;
int speed2 = 180;

const int trigPin = 6;
const int echoPin = 7;
long duration;
float distance;

void setup() {
  Serial.begin(9600);
  pinMode(m1a, OUTPUT);
  pinMode(m1b, OUTPUT);
  pinMode(m2a, OUTPUT);
  pinMode(m2b, OUTPUT);
  pinMode(sp1, OUTPUT);
  pinMode(sp2, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(A5,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(A7,INPUT_PULLUP);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void front() {
  digitalWrite(m1a, LOW); // left motor
  digitalWrite(m1b, HIGH);
  digitalWrite(m2a, HIGH); // right motor
  digitalWrite(m2b, LOW);
  analogWrite(sp1, speed);
  analogWrite(sp2, speed);
}

void stop() {
  digitalWrite(m1b, LOW);
  digitalWrite(m1a, LOW);
  digitalWrite(m2b, LOW);
  digitalWrite(m2a, LOW);
}

void right() {
  digitalWrite(m1a, HIGH);
  digitalWrite(m1b, LOW);
  digitalWrite(m2b, LOW);
  digitalWrite(m2a, HIGH);
  analogWrite(sp1, speed2);
  analogWrite(sp2, speed2);
}

void left() {
  digitalWrite(m1a, LOW);
  digitalWrite(m1b, HIGH);
  digitalWrite(m2b, HIGH);
  digitalWrite(m2a, LOW);
  analogWrite(sp1, speed2);
  analogWrite(sp2, speed2);
}

void loop() {
  ir1 = digitalRead(A0);
  ir2 = digitalRead(A1);
  ir3 = digitalRead(A2);
  ir4 = digitalRead(A3);
  ir5 = digitalRead(A4);
  s1 = digitalRead(A5);
  s2 = digitalRead(12);
  s3 = digitalRead(A7);

  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = duration * 0.034 / 2;
  Serial.println(s2);

  if(s2 == 0){
    right();
    delay(2000);
  }
  if(s1 == 0){
    front();
    delay(600);
  }
  else{
    if(ir5 == 1){
      if(distance>20){
        right();
        delay(800);
        front();
        delay(400);
      }
    }
    else if(ir5 == 0){
      if(distance>20){
        digitalWrite(11, LOW);
        if (ir1 == 0 && ir4 == 0 && ir2 == 0 && ir3 == 0) {
          front();
        } 
        else if (ir1 == 1 && ir4 == 1 && ir2 == 1 && ir3 == 1) {
          stop();
        } 
        else if (ir4 == 1 || ir3 == 1) {
          left();
        } 
        else if (ir1 == 1 || ir2 == 1) {
          right();
        } 
        else {
          stop();
        }
      }
      else{
        stop();
        analogWrite(11, 150);
      }

    }
  }
  delay(10);
}
                    
                
