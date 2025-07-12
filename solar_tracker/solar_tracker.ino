    #include <Servo.h>
    int servopin1=6; //left and right
    int servopin2=9; // up and down
    int servopin3=11; // rotate
    int i=90;
    int j=90;
    Servo myservo1;
    Servo myservo2;
    Servo myservo3;
    int ldr1; //down
    int ldr2; //right
    int ldr3; //up
    int ldr4; //left

    int a=0;


    void setup() {
      // put your setup code here, to run once:
      Serial.begin(9600);
      myservo1.attach(servopin1);
      myservo2.attach(servopin2);
      myservo3.attach(servopin3);
    }

    void loop() {
      // put your main code here, to run repeatedly:
      myservo1.write(i); //max=150(left) and min=30(right) 90 standard
      myservo2.write(j); //max=130(up) and min=70(down) 100 standard

      ldr1=analogRead(A0);
      Serial.print("ldr1=");
      Serial.println(ldr1);

      ldr2=analogRead(A1);
      Serial.print("ldr2=");
      Serial.println(ldr2);

      ldr3=analogRead(A2);
      Serial.print("ldr3=");
      Serial.println(ldr3);

      ldr4=analogRead(A3);
      Serial.print("ldr4=");
      Serial.println(ldr4);

      if(ldr1>500 & ldr2>500 & ldr3>500 & ldr4>500 & a==0){
        myservo3.write(130);
        delay(3050);
        myservo3.write(90);
        a=a+1;
        delay(200);
      }
      else if(ldr1<500 & ldr3<500 & a==1){
        myservo3.write(55);
        delay(3950);
        myservo3.write(90);
        a=a-1;
        delay(200);
      }


      if(ldr1>ldr3 & ldr1>650 & j>40){
        j=j-2;//down
      }
      else if(ldr3>ldr1 & ldr3>650 & j<150){
        j=j+2;//up
      }
      else if(ldr2>ldr4 & ldr2>650 & i<170 & j<100){
        i=i+2;//
      }
      else if(ldr4>ldr2 & ldr4>650 & i>10 & j<100){
        i=i-2;
      }
      else if(ldr2>ldr4 & ldr2>650 & i<170 & j>100){
        i=i-2;
      }
      else if(ldr4>ldr2 & ldr4>650 & i>10 & j>100){
        i=i+2;
      }
      delay(10);
    }
                            
                