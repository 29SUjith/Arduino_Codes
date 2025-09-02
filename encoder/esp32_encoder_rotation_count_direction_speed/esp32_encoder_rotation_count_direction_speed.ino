#define ENC_A 22  // Encoder Channel A
#define ENC_B 23  // Encoder Channel B (added for direction)
#define PULSES_PER_REV 280  // Corrected value

volatile long encoderValue = 0;
unsigned long lastPulseTime = 0;
const int debounceTime = 500; // Microseconds debounce

// Variables for RPM calculation
unsigned long lastRpmCalcTime = 0;
long prevEncoderValue = 0;
float rpm = 0.0;

void IRAM_ATTR updateEncoder() {
  unsigned long currentTime = micros();
  if (currentTime - lastPulseTime > debounceTime) {
    // Read the current state of both encoder channels
    int A_state = digitalRead(ENC_A);
    int B_state = digitalRead(ENC_B);
    
    // Determine direction based on the quadrature signals
    // If A leads B, clockwise rotation
    // If B leads A, counterclockwise rotation
    if (A_state == B_state) {
      encoderValue++;  // Clockwise
    } else {
      encoderValue--;  // Counterclockwise
    }
    
    lastPulseTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ENC_A, INPUT_PULLUP);
  pinMode(ENC_B, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), updateEncoder, CHANGE);
  
  // Initialize timing variables
  lastRpmCalcTime = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  float rotations = (float)encoderValue / PULSES_PER_REV;
  
  // Calculate RPM every 500ms
  if (currentMillis - lastRpmCalcTime >= 500) {
    // Calculate time elapsed in minutes
    float timeElapsed = (currentMillis - lastRpmCalcTime) / 60000.0;
    
    // Calculate pulses during this period
    long pulsesDelta = encoderValue - prevEncoderValue;
    
    // Calculate rotations during this period
    float rotationsDelta = (float)pulsesDelta / PULSES_PER_REV;
    
    // Calculate RPM
    rpm = rotationsDelta / timeElapsed;
    
    // Update variables for next calculation
    prevEncoderValue = encoderValue;
    lastRpmCalcTime = currentMillis;
  }

  // Determine direction text
  String direction = "STOPPED";
  if (rpm > 0.5) {
    direction = "CLOCKWISE";
  } else if (rpm < -0.5) {
    direction = "COUNTER-CLOCKWISE";
  }
  
  // Display absolute RPM value
  float absRpm = rpm < 0 ? -rpm : rpm;
  
  // Print all information
  Serial.print("Pulses: ");
  Serial.print(encoderValue);
  Serial.print(" | Rotations: ");
  Serial.print(rotations, 3);
  Serial.print(" | Speed: ");
  Serial.print(absRpm, 1);
  Serial.print(" RPM | Direction: ");
  Serial.println(direction);

  delay(100);  // More frequent updates
}