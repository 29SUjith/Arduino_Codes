#include <Servo.h> 
Servo myServo; 

int ldr1;
int ldr2;

int angle=90;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  myServo.attach(9);
}

void loop() {
  // put your main code here, to run repeatedly:
  myServo.write(angle);
  ldr1=digitalRead(A0);
  ldr2=digitalRead(A1);
  //Serial.println(ldr1);
  //Serial.println(ldr2);
  

  if(ldr1==0 && ldr2==1){
    angle=angle+5;
  }
  else if(ldr1==1 && ldr2==0){
    angle=angle-5;
  }
  Serial.println(angle);
  delay(50);
}
