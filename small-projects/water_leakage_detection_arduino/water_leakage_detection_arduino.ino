volatile int flowCount1 = 0;
volatile int flowCount2 = 0;

float calibrationFactor = 4.5; // pulses per second per L/min

void flow1() { flowCount1++; }
void flow2() { flowCount2++; }

void setup() {
  Serial.begin(9600);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(2), flow1, RISING);
  attachInterrupt(digitalPinToInterrupt(3), flow2, RISING);
}

void loop() {
  flowCount1 = 0;
  flowCount2 = 0;
  interrupts();
  delay(1000);  // 1 second sampling
  noInterrupts();

  float flowRate1 = (flowCount1 / calibrationFactor);
  float flowRate2 = (flowCount2 / calibrationFactor);

  Serial.print("Inlet: ");
  Serial.print(flowRate1);
  Serial.print(" L/min | Outlet: ");
  Serial.print(flowRate2);
  Serial.print(" L/min");

  float diff = abs(flowRate1 - flowRate2);
  if (diff > 0.5) {
    Serial.println(" >> LEAK DETECTED!");
  } else {
    Serial.println(" >> No Leak");
  }
}
