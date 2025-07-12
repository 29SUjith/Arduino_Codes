#define CH3 6
#define CH6 11
 
int ch3Value;
bool ch6Value;

int ma1=2;
int ma2=3;
int mb1=4;
int mb2=5;
int sp1=9;
int sp2=10;

int readChannel(int channelInput, int minLimit, int maxLimit, int defaultValue){
  int ch = pulseIn(channelInput, HIGH, 30000);
  if (ch < 100) return defaultValue;
  return map(ch, 1000, 2000, minLimit, maxLimit);
}
 
bool readSwitch(byte channelInput, bool defaultValue){
  int intDefaultValue = (defaultValue)? 100: 0;
  int ch = readChannel(channelInput, 0, 100, intDefaultValue);
  return (ch > 50);
}
 
void setup(){
 
  Serial.begin(115200);
  
  pinMode(CH3, INPUT);
  pinMode(CH6, INPUT);

  pinMode(ma1, OUTPUT);
  pinMode(ma2, OUTPUT);
  pinMode(mb1, OUTPUT);
  pinMode(mb2, OUTPUT);
  pinMode(sp1, OUTPUT);
  pinMode(sp2, OUTPUT);
}
 
void front(){
  digitalWrite(ma1,HIGH);
  digitalWrite(ma2,LOW);
  digitalWrite(mb1,HIGH);
  digitalWrite(mb2,LOW);
}


void stop(){
  digitalWrite(ma1,LOW);
  digitalWrite(ma2,LOW);
  digitalWrite(mb1,LOW);
  digitalWrite(mb2,LOW);
}

void back(){
  digitalWrite(ma1,LOW);
  digitalWrite(ma2,HIGH);
  digitalWrite(mb1,LOW);
  digitalWrite(mb2,HIGH);
}

void loop() {
  
  // Get values for each channel

  ch3Value = readChannel(CH3, 0, 250, 0);
  ch6Value = readSwitch(CH6, false);
  
  // Print to Serial Monitor

  Serial.print(" | Ch3: ");
  Serial.print(ch3Value);
  Serial.print(" | Ch6: ");
  Serial.println(ch6Value);
  
  //
  if(ch6Value==1){
    
      if(ch3Value>10){
        front();
        Serial.println("front");
        analogWrite(sp1,ch3Value);
        analogWrite(sp2,ch3Value);
      }
      else{
        stop();
      }

  }
  else if(ch6Value==0){
     
      if(ch3Value>10){
        back();
        Serial.println("back");
        analogWrite(sp1,ch3Value);
        analogWrite(sp2,ch3Value);
      }
       else{
        stop();
      }

  }
  delay(200);

}