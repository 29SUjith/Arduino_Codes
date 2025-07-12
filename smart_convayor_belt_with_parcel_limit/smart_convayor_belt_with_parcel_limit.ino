#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

int North = 0;
int East = 0;
int West = 0;
int parcelCount = 0; // Counter for detected parcels

const int buttonPin = 4;  // Push button

const int motorPin = 8;   // Motor control pin
const int motorPin1 = 9;  // Motor control pin
const int speedpin = 5;   // PWM Speed control pin

const int motor2Pin = 10; // Motor 2 control pin
const int motor2Pin1 = 11;// Motor 2 control pin

const int trigPin = 12;  // Ultrasonic sensor Trig pin
const int echoPin = 13;  // Ultrasonic sensor Echo pin

int speed = 200;
bool motorEnabled = true;  // Flag to control motor state

Servo myServo;  // Servo object

// Set LCD I2C address and size
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
    Serial.begin(9600);  
    lcd.begin(16, 2);    
    lcd.backlight();     
    lcd.setCursor(0, 0);
    lcd.print("W=0 N=0 E=0");  

    myServo.attach(6);
    myServo.write(90);  

    pinMode(buttonPin, INPUT_PULLUP); 
    pinMode(motorPin, OUTPUT);
    pinMode(motorPin1, OUTPUT);
    pinMode(speedpin, OUTPUT);
    pinMode(motor2Pin, OUTPUT);
    pinMode(motor2Pin1, OUTPUT);

    pinMode(trigPin, OUTPUT); // Ultrasonic sensor
    pinMode(echoPin, INPUT);

    digitalWrite(motorPin, LOW); 
    digitalWrite(motorPin1, LOW);
    digitalWrite(motor2Pin, LOW);
    digitalWrite(motor2Pin1, HIGH);
}

float getDistance() {
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);

    long duration = pulseIn(echoPin, HIGH);
    return duration * 0.034 / 2; // Convert to cm
}

void moveServo(int targetAngle) {
    int currentAngle = myServo.read();
    if (currentAngle < targetAngle) {
        for (int pos = currentAngle; pos <= targetAngle; pos++) {
            myServo.write(pos);
            delay(20);  // Smooth transition
        }
    } else {
        for (int pos = currentAngle; pos >= targetAngle; pos--) {
            myServo.write(pos);
            delay(20);  // Smooth transition
        }
    }
}

void stopAll() {
    digitalWrite(motorPin, LOW);
    digitalWrite(motorPin1, LOW);
    digitalWrite(motor2Pin, LOW);
    digitalWrite(motor2Pin1, LOW);
    analogWrite(speedpin, 0);
    myServo.write(90); // Reset servo
    motorEnabled = false;
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Limit Reached!");
    lcd.setCursor(0, 1);
    lcd.print("Stopping System");
    
    Serial.println("20 parcels detected. System stopped.");
}

void loop() {
    if (parcelCount >= 20) {
        stopAll(); // Stop all operations when 20 parcels are detected
        return;    // Prevent further execution of loop()
    }

    float distance = getDistance();

    if (distance < 5) {  // Stop motor if an obstacle is detected within 5 cm
        digitalWrite(motorPin, LOW);
        digitalWrite(motorPin1, LOW);
        digitalWrite(motor2Pin, LOW);
        digitalWrite(motor2Pin1, LOW);
        analogWrite(speedpin, 0);
        lcd.setCursor(0, 1);
        lcd.print("Obstacle! Stop ");
        motorEnabled = false;
        
        if (Serial.available() > 0) {  // Resume motor on serial input
            String shortcode = Serial.readStringUntil('\n');
            shortcode.trim();

            if (shortcode == "N" || shortcode == "E" || shortcode == "W") {
                parcelCount++; // Increase parcel count on valid command
                Serial.print("Parcel Count: ");
                Serial.println(parcelCount);
            }

            if (shortcode == "N") {
                North++;
                delay(100);
                digitalWrite(motorPin, HIGH);  
                digitalWrite(motorPin1, LOW);
                analogWrite(speedpin, speed);
                delay(5000);
                moveServo(90);  
                delay(200);
                digitalWrite(motor2Pin, LOW);
                digitalWrite(motor2Pin1, HIGH);
                delay(4000);
            } 
            else if (shortcode == "E") {
                East++;
                delay(100);
                digitalWrite(motorPin, HIGH);  
                digitalWrite(motorPin1, LOW);
                analogWrite(speedpin, speed);
                delay(5000);
                moveServo(0);
                delay(200);
                digitalWrite(motor2Pin, LOW);
                digitalWrite(motor2Pin1, HIGH);
                delay(4000);
            } 
            else if (shortcode == "W") {
                West++;
                delay(100);
                digitalWrite(motorPin, HIGH);  
                digitalWrite(motorPin1, LOW);
                analogWrite(speedpin, speed);
                delay(5000);
                moveServo(180);
                delay(200);
                digitalWrite(motor2Pin, LOW);
                digitalWrite(motor2Pin1, HIGH);
                delay(4000);
            }

            moveServo(90); // Reset servo

            // Update LCD
            lcd.setCursor(0, 0);
            lcd.print("W=");
            lcd.print(West);
            lcd.print(" N=");
            lcd.print(North);
            lcd.print(" E=");
            lcd.print(East);
            lcd.print(" ");
            
            Serial.println("Received: " + shortcode);
            motorEnabled = true;
        }
    } 

    // Button-controlled motor operation
    lcd.setCursor(0, 1);
    if (digitalRead(buttonPin) == LOW && motorEnabled) {  
        digitalWrite(motorPin, HIGH);  
        digitalWrite(motorPin1, LOW);
        analogWrite(speedpin, speed);
        digitalWrite(motor2Pin, LOW);
        digitalWrite(motor2Pin1, HIGH);
        lcd.print("Motor: ON  ");
    } 
    else {
        digitalWrite(motorPin, LOW);
        digitalWrite(motorPin1, LOW);
        analogWrite(speedpin, 0);
        digitalWrite(motor2Pin, LOW);
        digitalWrite(motor2Pin1, LOW);
        lcd.print("Motor: OFF ");
    }
}
