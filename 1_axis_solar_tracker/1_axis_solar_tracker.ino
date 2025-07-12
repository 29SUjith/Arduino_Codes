#include <Servo.h>
Servo myServo;
int ldr1;
int ldr2;
int i=90;
void setup() {

  myServo.attach(9);
  Serial.begin(9600);
}

void loop() {
  ldr1=analogRead(A0);
  Serial.print("ldr1=");
  Serial.println(ldr1);
  ldr2=analogRead(A1);
  Serial.print("ldr2=");
  Serial.println(ldr2);

  if(ldr1>ldr2 && ldr1>500){
    i=i+3;
  }
  else if(ldr2>ldr1 && ldr2>700){
    i=i-3;
  }
  myServo.write(i); 

  delay(50);                 
}
