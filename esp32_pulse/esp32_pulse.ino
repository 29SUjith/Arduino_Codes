#include <PulseSensorPlayground.h>

#define PULSE_SENSOR_PIN 34  // Analog pin connected to pulse sensor
#define THRESHOLD 550        // Adjust based on your sensor reading

PulseSensorPlayground pulseSensor;  // Create PulseSensor object

void setup() {
  Serial.begin(115200);

  // Configure the pulse sensor
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.setThreshold(THRESHOLD);

  if (pulseSensor.begin()) {
    Serial.println("Pulse sensor initialized!");
  } else {
    Serial.println("Failed to initialize pulse sensor.");
  }
}

void loop() {
  int myBPM = pulseSensor.getBeatsPerMinute();  // Get BPM value

  if (pulseSensor.sawStartOfBeat()) {  // If a new beat is detected
    Serial.print("BPM: ");
    Serial.println(myBPM);
  }

  delay(20); // Small delay to stabilize readings
}
