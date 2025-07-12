#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define calibration_factor -70500.0 //This value is obtained using the SparkFun_HX711_Calibration sketch

#define LOADCELL_DOUT_PIN  9
#define LOADCELL_SCK_PIN  10

HX711 scale;

void setup() {
  Serial.begin(9600);
  Serial.println("HX711 scale demo");

  scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
  scale.set_scale(calibration_factor); // This value is obtained by using the SparkFun_HX711_Calibration sketch
  scale.tare();  // Assuming there is no weight on the scale at start up, reset the scale to 0

  Serial.println("Readings:");
}

void loop() {
  float lbs = scale.get_units(10);  // Take average of 10 readings to smooth noise
  float kg = lbs * 0.453592;  // Convert lbs to kg

  Serial.print("Reading: ");
  Serial.print(kg, 2);  // Print result in kg with 2 decimal places
  Serial.println(" kg");

  delay(500);  // Delay to make output readable
}
