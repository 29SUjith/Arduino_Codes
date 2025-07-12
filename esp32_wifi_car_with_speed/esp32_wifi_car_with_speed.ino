#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Project";
const char* password = "12345678";

// Web server running on port 80
WebServer server(80);

// Motor control pins
const int IN1 = 5; // Motor A
const int IN2 = 18;  // Motor A
const int ENA = 15; // Motor A PWM
const int IN3 = 19;  // Motor B
const int IN4 = 21;  // Motor B
const int ENB = 4; // Motor B PWM

int speedValue = 255; // Default speed (range 0-255)

// HTML page
const char* htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP32 Car Control</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
      background-color: #f4f4f9;
      text-align: center;
    }
    h1 {
      margin: 20px 0;
      font-size: 2.5rem;
      color: #333;
    }
    .container {
      max-width: 800px;
      margin: 10px auto;
      margin-top: 40px;
      padding: 20px;
      background: #fff;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    }
    .control-layout {
      display: grid;
      grid-template-rows: repeat(3, 140px);
      grid-template-columns: repeat(3, 140px);
      gap: 10px;
      justify-content: center;
      align-items: center;
      margin-bottom: 20px;
    }
    .control-layout button {
      font-size: 25px;
      font-weight: bold;
      color: #fff;
      background: #007BFF;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background 0.3s ease;
      padding: 15px;
    }
    .control-layout button:hover {
      background: #0056b3;
    }
    .control-layout button.empty {
      background: transparent;
      cursor: default;
    }
    .speed_text {
      font-size: 24px;
    }
    input[type="range"] {
      width: 100%;
      margin: 20px 0;
      height: 5px;
    }
    .speed-display {
      font-size: 20px;
      color: #555;
    }
    footer {
      padding-top: 40px;
      font-size: 44px;
      color: #666;
    }
    .company-name {
      font-weight: bold;
      color: #333;
      font-size: 55px;
      margin-top: 10px;
    }
  </style>
</head>
<body>
  <h1>ESP32 Car Control</h1>
  <div class="container">
    <div class="control-layout">
      <button class="empty"></button>
      <button onclick="sendCommand('F')">Forward</button>
      <button class="empty"></button>
      <button onclick="sendCommand('L')">Left</button>
      <button onclick="sendCommand('S')">Stop</button>
      <button onclick="sendCommand('R')">Right</button>
      <button class="empty"></button>
      <button onclick="sendCommand('B')">Backward</button>
      <button class="empty"></button>
    </div>
    <label class="speed_text" for="speed">Speed:</label>
    <input type="range" id="speed" min="0" max="255" value="255" oninput="updateSpeed(this.value)">
    <p class="speed-display">Current Speed: <span id="speedValue">255</span></p>
  </div>
  <footer>
    <p>Page crafted with innovation and precision by:</p>
    <p class="company-name">M S Innovations</p>
  </footer>
  <script>
    function sendCommand(command) {
      fetch(`/control?cmd=${command}`).catch((err) => console.error('Failed to send command', err));
    }
    function updateSpeed(value) {
      document.getElementById('speedValue').innerText = value;
      fetch(`/speed?value=${value}`).catch((err) => console.error('Failed to update speed', err));
    }
  </script>
</body>
</html>   
)rawliteral";

void setup() {
  Serial.begin(115200);

  // Setup pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
  stopCar();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/", []() { server.send(200, "text/html", htmlPage); });
  server.on("/control", handleControl);
  server.on("/speed", handleSpeed);

  // Start server
  server.begin();
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
}

// Handle movement commands
void handleControl() {
  String cmd = server.arg("cmd");
  if (cmd == "F") forward();
  else if (cmd == "B") backward();
  else if (cmd == "L") left();
  else if (cmd == "R") right();
  else if (cmd == "S") stopCar();
  server.send(200, "text/plain", "OK");
}

// Handle speed change
void handleSpeed() {
  speedValue = server.arg("value").toInt();
  server.send(200, "text/plain", "Speed updated");
}

// Motor control functions
void forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void backward() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue);
}

void right() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, speedValue / 2);
  analogWrite(ENB, speedValue);
}

void left() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  analogWrite(ENA, speedValue);
  analogWrite(ENB, speedValue / 2);
}

void stopCar() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
}
