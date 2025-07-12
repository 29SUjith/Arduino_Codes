#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

SoftwareSerial mp3Serial(10, 11); // RX, TX
DFRobotDFPlayerMini mp3;

const int buttonPin = 2;
bool buttonPressed = false;

void setup() {
  Serial.begin(115200);
  mp3Serial.begin(9600);

  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);

  if (!mp3.begin(mp3Serial)) {
    Serial.println("MP3 module not found!");
    while (true);
  }

  mp3.volume(45);  // Set volume (0 to 30)
}

void loop() {
  if (Serial.available()) {
    String received = Serial.readStringUntil('\n');
    received.trim();

    // Map object to audio index
    if (received == "truck")        mp3.play(1);
    else if (received == "tree")     mp3.play(2);
    else if (received == "person")  mp3.play(3);
    else if (received == "car")     mp3.play(4);
    else if (received == "pothole") mp3.play(5);
    else if (received == "pole")    mp3.play(6);
    else if (received == "bike")    mp3.play(7);

    Serial.println("Object detected: " + received);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (digitalRead(buttonPin) == LOW && !buttonPressed) {
    buttonPressed = true;
    Serial.println("SEND_EMAIL");
    delay(500);
  }

  if (digitalRead(buttonPin) == HIGH) {
    buttonPressed = false;
  }
}