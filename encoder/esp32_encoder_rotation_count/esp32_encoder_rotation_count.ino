#define ENC_A 15  // Encoder Channel A
#define PULSES_PER_REV 280  // New corrected value

volatile long encoderValue = 0;
unsigned long lastPulseTime = 0;
const int debounceTime = 500; // Microseconds debounce

void IRAM_ATTR updateEncoder() {
  unsigned long currentTime = micros();
  if (currentTime - lastPulseTime > debounceTime) {  
    encoderValue++;  
    lastPulseTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ENC_A, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(ENC_A), updateEncoder, CHANGE);  
}

void loop() {
  float rotations = (float)encoderValue / PULSES_PER_REV;  

  Serial.print("Pulses: ");
  Serial.print(encoderValue);
  Serial.print(" | Rotations: ");
  Serial.println(rotations);

  delay(500);
}
