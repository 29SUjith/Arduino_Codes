#include <WiFi.h>
#include <WebServer.h>

// WiFi credentials
const char* ssid = "Projects";
const char* password = "12345678@";

// Create web server on port 80
WebServer server(80);

// L298N Motor Driver pins
const int motor1Pin1 = 13;  // IN1
const int motor1Pin2 = 12;  // IN2
const int motor2Pin1 = 14;  // IN3
const int motor2Pin2 = 27;  // IN4
const int enable1Pin = 18;  // ENA
const int enable2Pin = 5;  // ENB

// Web page HTML
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <title>ESP32 WiFi Car</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      font-family: Arial, sans-serif;
      text-align: center;
      margin: 0;
      padding: 20px;
      background-color: #1e1e1e;
      color: white;
    }
    .container {
      max-width: 400px;
      margin: 0 auto;
      background-color: #2d2d2d;
      padding: 20px;
      border-radius: 15px;
      box-shadow: 0 4px 15px rgba(0,0,0,0.3);
    }
    h1 {
      color: #00d4ff;
      margin-bottom: 30px;
    }
    .controls {
      display: grid;
      grid-template-columns: 1fr 1fr 1fr;
      grid-template-rows: 1fr 1fr 1fr;
      gap: 15px;
      margin: 20px 0;
      height: 250px;
    }
    .btn {
      background: linear-gradient(145deg, #4a90e2, #357abd);
      border: none;
      border-radius: 12px;
      color: white;
      font-size: 18px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s;
      box-shadow: 0 4px 8px rgba(0,0,0,0.2);
    }
    .btn:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 12px rgba(0,0,0,0.3);
    }
    .btn:active {
      transform: translateY(0);
      box-shadow: 0 2px 4px rgba(0,0,0,0.2);
      background: linear-gradient(145deg, #357abd, #2868a3);
    }
    .forward { grid-column: 2; grid-row: 1; }
    .left { grid-column: 1; grid-row: 2; }
    .stop { 
      grid-column: 2; 
      grid-row: 2; 
      background: linear-gradient(145deg, #e74c3c, #c0392b);
      font-size: 16px;
    }
    .stop:active {
      background: linear-gradient(145deg, #c0392b, #a93226);
    }
    .right { grid-column: 3; grid-row: 2; }
    .backward { grid-column: 2; grid-row: 3; }
    .status {
      margin-top: 20px;
      padding: 15px;
      background-color: #333;
      border-radius: 8px;
      font-size: 14px;
    }
    .ip-info {
      margin-top: 10px;
      color: #00d4ff;
      font-size: 12px;
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🚗 ESP32 WiFi Car</h1>
    
    <div class="controls">
      <button class="btn forward" onmousedown="sendCommand('forward')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('forward')" ontouchend="sendCommand('stop')">▲<br>FORWARD</button>
      <button class="btn left" onmousedown="sendCommand('left')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('left')" ontouchend="sendCommand('stop')">◄<br>LEFT</button>
      <button class="btn stop" onclick="sendCommand('stop')">⏹<br>STOP</button>
      <button class="btn right" onmousedown="sendCommand('right')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('right')" ontouchend="sendCommand('stop')">►<br>RIGHT</button>
      <button class="btn backward" onmousedown="sendCommand('backward')" onmouseup="sendCommand('stop')" ontouchstart="sendCommand('backward')" ontouchend="sendCommand('stop')">▼<br>BACKWARD</button>
    </div>
    
    <div class="status">
      <div>Status: <span id="carStatus">Stopped</span></div>
      <div class="ip-info">Car IP: <span id="carIP">---.---.---.---</span></div>
    </div>
  </div>

  <script>
    // Get car IP address
    fetch('/ip')
      .then(response => response.text())
      .then(ip => document.getElementById('carIP').textContent = ip);

    function sendCommand(command) {
      var xhr = new XMLHttpRequest();
      xhr.open('GET', '/' + command, true);
      xhr.send();
      
      // Update status display
      var statusElement = document.getElementById('carStatus');
      switch(command) {
        case 'forward':
          statusElement.textContent = 'Moving Forward';
          statusElement.style.color = '#2ecc71';
          break;
        case 'backward':
          statusElement.textContent = 'Moving Backward';
          statusElement.style.color = '#f39c12';
          break;
        case 'left':
          statusElement.textContent = 'Turning Left';
          statusElement.style.color = '#3498db';
          break;
        case 'right':
          statusElement.textContent = 'Turning Right';
          statusElement.style.color = '#9b59b6';
          break;
        case 'stop':
          statusElement.textContent = 'Stopped';
          statusElement.style.color = '#e74c3c';
          break;
      }
    }

    // Prevent touch events from causing unwanted behaviors
    document.addEventListener('touchstart', function(e) {
      if (e.target.classList.contains('btn')) {
        e.preventDefault();
      }
    }, {passive: false});
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Set motor pins as outputs
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(motor2Pin1, OUTPUT);
  pinMode(motor2Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  pinMode(enable2Pin, OUTPUT);
  
  // Set enable pins to HIGH for full speed
  digitalWrite(enable1Pin, HIGH);
  digitalWrite(enable2Pin, HIGH);
  
  // Initialize all motors to stop
  stopMotors();
  
  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Define web server routes
  server.on("/", handleRoot);
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/ip", handleIP);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// Web page handler
void handleRoot() {
  server.send(200, "text/html", index_html);
}

// Motor control functions
void handleForward() {
  moveForward();
  server.send(200, "text/plain", "Moving Forward");
  Serial.println("Command: Forward");
}

void handleBackward() {
  moveBackward();
  server.send(200, "text/plain", "Moving Backward");
  Serial.println("Command: Backward");
}

void handleLeft() {
  turnLeft();
  server.send(200, "text/plain", "Turning Left");
  Serial.println("Command: Left");
}

void handleRight() {
  turnRight();
  server.send(200, "text/plain", "Turning Right");
  Serial.println("Command: Right");
}

void handleStop() {
  stopMotors();
  server.send(200, "text/plain", "Stopped");
  Serial.println("Command: Stop");
}

void handleIP() {
  server.send(200, "text/plain", WiFi.localIP().toString());
}

// Motor movement functions
void moveForward() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void moveBackward() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void turnLeft() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, HIGH);
  digitalWrite(motor2Pin1, HIGH);
  digitalWrite(motor2Pin2, LOW);
}

void turnRight() {
  digitalWrite(motor1Pin1, HIGH);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, HIGH);
}

void stopMotors() {
  digitalWrite(motor1Pin1, LOW);
  digitalWrite(motor1Pin2, LOW);
  digitalWrite(motor2Pin1, LOW);
  digitalWrite(motor2Pin2, LOW);
}