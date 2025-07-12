#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "Projects"; //change it to your ssid
const char* password = "12345678@";//change it to your password

// Flow sensor pins
#define FLOW_SENSOR1 18
#define FLOW_SENSOR2 19

volatile int flowCount1 = 0;
volatile int flowCount2 = 0;

float calibrationFactor = 4.5; // Pulses per second per L/min

WebServer server(80);
String htmlContent;  // Global HTML string

// Interrupt functions
void IRAM_ATTR flow1() { flowCount1++; }
void IRAM_ATTR flow2() { flowCount2++; }

void setup() {
  Serial.begin(115200);

  // Set pin modes
  pinMode(FLOW_SENSOR1, INPUT_PULLUP);
  pinMode(FLOW_SENSOR2, INPUT_PULLUP);

  // Attach interrupts
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR1), flow1, RISING);
  attachInterrupt(digitalPinToInterrupt(FLOW_SENSOR2), flow2, RISING);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  // Start web server
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  static unsigned long lastMillis = 0;
  static float flowRate1 = 0;
  static float flowRate2 = 0;
  static String leakStatus = "Checking...";
  static String statusColor = "#FFA500";
  static String statusIcon = "🔍";

  server.handleClient();  // Handle web client requests

  // Run once every second
  if (millis() - lastMillis >= 1000) {
    noInterrupts();  // Temporarily disable interrupts to safely read counts
    int count1 = flowCount1;
    int count2 = flowCount2;
    flowCount1 = 0;
    flowCount2 = 0;
    interrupts();  // Re-enable interrupts

    flowRate1 = count1 / calibrationFactor;
    flowRate2 = count2 / calibrationFactor;

    float diff = abs(flowRate1 - flowRate2);
    
    if (diff > 0.9) {
      leakStatus = "LEAK DETECTED!";
      statusColor = "#FF4444";
      statusIcon = "⚠️";
    } else {
      leakStatus = "System Normal";
      statusColor = "#00AA00";
      statusIcon = "✅";
    }

    // Print to serial monitor
    Serial.print("Inlet: ");
    Serial.print(flowRate1);
    Serial.print(" L/min | Outlet: ");
    Serial.print(flowRate2);
    Serial.print(" L/min >> ");
    Serial.println(leakStatus);

    // Update HTML content with enhanced styling
    htmlContent = "<!DOCTYPE html><html><head><meta charset='UTF-8'>"
                  "<meta http-equiv='refresh' content='1'/>"
                  "<meta name='viewport' content='width=device-width, initial-scale=1.0'>"
                  "<title>Flow Monitor Dashboard</title>"
                  "<style>"
                  "* { margin: 0; padding: 0; box-sizing: border-box; }"
                  "body { font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif; "
                  "background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); "
                  "min-height: 100vh; padding: 20px; }"
                  ".container { max-width: 800px; margin: 0 auto; }"
                  ".header { background: rgba(255,255,255,0.95); "
                  "border-radius: 15px; padding: 30px; margin-bottom: 20px; "
                  "box-shadow: 0 8px 32px rgba(0,0,0,0.1); "
                  "backdrop-filter: blur(10px); text-align: center; }"
                  ".title { color: #2c3e50; font-size: 2.5em; margin-bottom: 10px; "
                  "text-shadow: 2px 2px 4px rgba(0,0,0,0.1); }"
                  ".subtitle { color: #7f8c8d; font-size: 1.1em; }"
                  ".dashboard { display: grid; grid-template-columns: 1fr 1fr; "
                  "gap: 20px; margin-bottom: 20px; }"
                  ".sensor-card { background: rgba(255,255,255,0.95); "
                  "border-radius: 15px; padding: 25px; text-align: center; "
                  "box-shadow: 0 8px 32px rgba(0,0,0,0.1); "
                  "backdrop-filter: blur(10px); transition: transform 0.3s ease; }"
                  ".sensor-card:hover { transform: translateY(-5px); }"
                  ".sensor-title { color: #34495e; font-size: 1.3em; "
                  "margin-bottom: 15px; font-weight: 600; }"
                  ".flow-value { font-size: 2.2em; font-weight: bold; "
                  "margin-bottom: 5px; }"
                  ".inlet-value { color: #3498db; }"
                  ".outlet-value { color: #e74c3c; }"
                  ".unit { color: #7f8c8d; font-size: 0.8em; }"
                  ".status-card { background: rgba(255,255,255,0.95); "
                  "border-radius: 15px; padding: 30px; text-align: center; "
                  "box-shadow: 0 8px 32px rgba(0,0,0,0.1); "
                  "backdrop-filter: blur(10px); grid-column: 1 / -1; }"
                  ".status-icon { font-size: 3em; margin-bottom: 15px; }"
                  ".status-text { font-size: 1.8em; font-weight: bold; "
                  "margin-bottom: 10px; }"
                  ".status-description { color: #7f8c8d; font-size: 1em; }"
                  ".pulse { animation: pulse 2s infinite; }"
                  "@keyframes pulse { 0% { opacity: 1; } 50% { opacity: 0.7; } 100% { opacity: 1; } }"
                  ".flow-difference { margin-top: 15px; padding: 10px; "
                  "background: rgba(52,73,94,0.1); border-radius: 8px; }"
                  ".diff-label { color: #7f8c8d; font-size: 0.9em; margin-bottom: 5px; }"
                  ".diff-value { font-size: 1.3em; font-weight: bold; color: #2c3e50; }"
                  "@media (max-width: 600px) { "
                  ".dashboard { grid-template-columns: 1fr; } "
                  ".title { font-size: 2em; } }"
                  "</style>"
                  "</head><body>"
                  "<div class='container'>"
                  "<div class='header'>"
                  "<h1 class='title'>Flow Monitor Dashboard</h1>"
                  "<p class='subtitle'>Real-time Water Flow & Leak Detection System</p>"
                  "</div>"
                  "<div class='dashboard'>"
                  "<div class='sensor-card'>"
                  "<div class='sensor-title'>Inlet Sensor</div>"
                  "<div class='flow-value inlet-value'>" + String(flowRate1, 2) + "</div>"
                  "<div class='unit'>L/min</div>"
                  "</div>"
                  "<div class='sensor-card'>"
                  "<div class='sensor-title'>Outlet Sensor</div>"
                  "<div class='flow-value outlet-value'>" + String(flowRate2, 2) + "</div>"
                  "<div class='unit'>L/min</div>"
                  "</div>"
                  "<div class='status-card'>"
                  "<div class='status-icon " + ((leakStatus == "LEAK DETECTED!") ? "pulse" : "") + "'>" + statusIcon + "</div>"
                  "<div class='status-text' style='color: " + statusColor + ";'>" + leakStatus + "</div>"
                  "<div class='status-description'>" + 
                  ((leakStatus == "LEAK DETECTED!") ? "Immediate attention required!" : "All systems operating normally") + "</div>"
                  "<div class='flow-difference'>"
                  "<div class='diff-label'>Flow Difference</div>"
                  "<div class='diff-value'>" + String(abs(flowRate1 - flowRate2), 2) + " L/min</div>"
                  "</div>"
                  "</div>"
                  "</div>"
                  "</div>"
                  "</body></html>";

    lastMillis = millis();
  }
}

// Handle web request
void handleRoot() {
  server.send(200, "text/html", htmlContent);
}