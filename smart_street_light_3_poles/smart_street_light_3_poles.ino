int ir1=35;
int ir2=32;
int ir3=33;
int ldr1=23;

int led1=5;
int led2=18;
int led3=19;

void setup() {
  // put your setup code here, to run once:
  pinMode(ir1,INPUT);
  pinMode(ir2,INPUT);
  pinMode(ir3,INPUT);
  pinMode(ldr1,INPUT);
  pinMode(led1,OUTPUT);
  pinMode(led2,OUTPUT);
  pinMode(led3,OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  int ir1r=digitalRead(ir1);
  Serial.println(ir1r);

  int ir2r=digitalRead(ir2);
  Serial.println(ir2r);

  int ir3r=digitalRead(ir3);
  Serial.println(ir3r);

  int ldr1r=digitalRead(ldr1);
  Serial.println(ldr1r);

  if(ldr1r==1){
    if(ir1r==0){
      analogWrite(led1,55);
      analogWrite(led2,55);
      analogWrite(led3,255);
    }
    else if(ir2r==0){
      analogWrite(led1,55);
      analogWrite(led2,255);
      analogWrite(led3,55);
    }
    else if(ir3r==0){
      analogWrite(led1,255);
      analogWrite(led2,55);
      analogWrite(led3,55);
    }
    else{
      analogWrite(led1,55);
      analogWrite(led2,55);
      analogWrite(led3,55);
    }
  }
  else{
    analogWrite(led1,0);
    analogWrite(led2,0);
    analogWrite(led3,0);
  }


  delay(50);
}
