#include <WiFi.h>
#include <WebServer.h>

// Replace with your network credentials
const char* ssid = "Projects";
const char* password = "12345678@";


// Create WebServer object on port 80
WebServer server(80);

// Motor control pins (L298N)
#define MOTOR_A1 12  // Left motor forward
#define MOTOR_A2 14  // Left motor backward
#define MOTOR_B1 27  // Right motor forward
#define MOTOR_B2 26  // Right motor backward
#define MOTOR_A_SPEED 13  // Left motor speed (PWM)
#define MOTOR_B_SPEED 25  // Right motor speed (PWM)

// Relay pins (Active Low)
#define ROLLER_RELAY 5   // Mop roller motor relay
#define PUMP_RELAY 18    // Water pump relay

// Variables
int motorSpeed = 200;  // Default speed (0-255)
bool rollerState = false;
bool pumpState = false;

// HTML webpage
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <title>Cleaning Robot Control</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        body { font-family: Arial, sans-serif; text-align: center; margin: 0px auto; padding-top: 30px; background-color: #f0f0f0; }
        .container { max-width: 400px; margin: 0 auto; background: white; padding: 20px; border-radius: 10px; box-shadow: 0 4px 8px rgba(0,0,0,0.1); }
        .button { background-color: #4CAF50; border: none; color: white; padding: 15px 25px; text-align: center; text-decoration: none; display: inline-block; font-size: 16px; margin: 4px 2px; cursor: pointer; border-radius: 5px; width: 120px; }
        .button:hover { background-color: #45a049; }
        .button:active { background-color: #3e8e41; }
        .stop { background-color: #f44336; }
        .stop:hover { background-color: #da190b; }
        .toggle { background-color: #ff9800; }
        .toggle:hover { background-color: #e68900; }
        .toggle.active { background-color: #4CAF50; }
        .speed-container { margin: 20px 0; }
        .speed-slider { width: 80%; }
        h1 { color: #333; }
        .control-section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🤖 Cleaning Robot Control</h1>
        
        <div class="control-section">
            <h3>Movement Controls</h3>
            <div>
                <button class="button" onmousedown="sendCommand('forward')" onmouseup="sendCommand('stop')">↑ Forward</button>
            </div>
            <div>
                <button class="button" onmousedown="sendCommand('left')" onmouseup="sendCommand('stop')">← Left</button>
                <button class="button stop" onclick="sendCommand('stop')">⏹ Stop</button>
                <button class="button" onmousedown="sendCommand('right')" onmouseup="sendCommand('stop')">Right →</button>
            </div>
            <div>
                <button class="button" onmousedown="sendCommand('backward')" onmouseup="sendCommand('stop')">↓ Backward</button>
            </div>
        </div>

        <div class="control-section">
            <h3>Speed Control</h3>
            <div class="speed-container">
                <label for="speedSlider">Speed: <span id="speedValue">200</span></label><br>
                <input type="range" min="100" max="255" value="200" class="speed-slider" id="speedSlider" oninput="updateSpeed(this.value)">
            </div>
        </div>

        <div class="control-section">
            <h3>Cleaning Controls</h3>
            <button class="button toggle" id="rollerBtn" onclick="toggleRoller()">Roller: OFF</button>
            <button class="button toggle" id="pumpBtn" onclick="togglePump()">Pump: OFF</button>
        </div>
    </div>

    <script>
        function sendCommand(command) {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/" + command, true);
            xhr.send();
        }
        
        function updateSpeed(value) {
            document.getElementById("speedValue").innerHTML = value;
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/speed?value=" + value, true);
            xhr.send();
        }
        
        function toggleRoller() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/toggle_roller", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var btn = document.getElementById("rollerBtn");
                    if (xhr.responseText == "ON") {
                        btn.innerHTML = "Roller: ON";
                        btn.classList.add("active");
                    } else {
                        btn.innerHTML = "Roller: OFF";
                        btn.classList.remove("active");
                    }
                }
            };
            xhr.send();
        }
        
        function togglePump() {
            var xhr = new XMLHttpRequest();
            xhr.open("GET", "/toggle_pump", true);
            xhr.onreadystatechange = function() {
                if (xhr.readyState == 4 && xhr.status == 200) {
                    var btn = document.getElementById("pumpBtn");
                    if (xhr.responseText == "ON") {
                        btn.innerHTML = "Pump: ON";
                        btn.classList.add("active");
                    } else {
                        btn.innerHTML = "Pump: OFF";
                        btn.classList.remove("active");
                    }
                }
            };
            xhr.send();
        }
    </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  
  // Initialize motor pins
  pinMode(MOTOR_A1, OUTPUT);
  pinMode(MOTOR_A2, OUTPUT);
  pinMode(MOTOR_B1, OUTPUT);
  pinMode(MOTOR_B2, OUTPUT);
  pinMode(MOTOR_A_SPEED, OUTPUT);
  pinMode(MOTOR_B_SPEED, OUTPUT);
  
  // Initialize relay pins (Active Low)
  pinMode(ROLLER_RELAY, OUTPUT);
  pinMode(PUMP_RELAY, OUTPUT);
  
  // Turn off all motors and relays initially
  stopMotors();
  digitalWrite(ROLLER_RELAY, HIGH);  // HIGH = OFF for active low relay
  digitalWrite(PUMP_RELAY, HIGH);    // HIGH = OFF for active low relay
  
  // Set PWM pins for motor speed control
  // analogWrite will be used for speed control
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println();
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());
  
  // Route for root / web page
  server.on("/", handleRoot);
  
  // Routes for robot control
  server.on("/forward", handleForward);
  server.on("/backward", handleBackward);
  server.on("/left", handleLeft);
  server.on("/right", handleRight);
  server.on("/stop", handleStop);
  server.on("/speed", handleSpeed);
  server.on("/toggle_roller", handleToggleRoller);
  server.on("/toggle_pump", handleTogglePump);
  
  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  server.handleClient();
}

// Web server handlers
void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleForward() {
  moveForward();
  server.send(200, "text/plain", "Moving forward");
}

void handleBackward() {
  moveBackward();
  server.send(200, "text/plain", "Moving backward");
}

void handleLeft() {
  turnLeft();
  server.send(200, "text/plain", "Turning left");
}

void handleRight() {
  turnRight();
  server.send(200, "text/plain", "Turning right");
}

void handleStop() {
  stopMotors();
  server.send(200, "text/plain", "Stopped");
}

void handleSpeed() {
  if (server.hasArg("value")) {
    motorSpeed = server.arg("value").toInt();
    motorSpeed = constrain(motorSpeed, 100, 255);  // Limit speed range
    server.send(200, "text/plain", "Speed set to " + String(motorSpeed));
  } else {
    server.send(400, "text/plain", "Bad Request");
  }
}

void handleToggleRoller() {
  rollerState = !rollerState;
  digitalWrite(ROLLER_RELAY, rollerState ? LOW : HIGH);  // LOW = ON for active low
  server.send(200, "text/plain", rollerState ? "ON" : "OFF");
}

void handleTogglePump() {
  pumpState = !pumpState;
  digitalWrite(PUMP_RELAY, pumpState ? LOW : HIGH);  // LOW = ON for active low
  server.send(200, "text/plain", pumpState ? "ON" : "OFF");
}

void turnLeft() {
  digitalWrite(MOTOR_A1, HIGH);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, HIGH);
  digitalWrite(MOTOR_B2, LOW);
  analogWrite(MOTOR_A_SPEED, motorSpeed);
  analogWrite(MOTOR_B_SPEED, motorSpeed);
  Serial.println("Moving Forward");
}

void turnRight() {
  digitalWrite(MOTOR_A1, LOW);
  digitalWrite(MOTOR_A2, HIGH);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, HIGH);
  analogWrite(MOTOR_A_SPEED, motorSpeed);
  analogWrite(MOTOR_B_SPEED, motorSpeed);
  Serial.println("Moving Backward");
}

void moveBackward() {
  digitalWrite(MOTOR_A1, LOW);
  digitalWrite(MOTOR_A2, HIGH);
  digitalWrite(MOTOR_B1, HIGH);
  digitalWrite(MOTOR_B2, LOW);
  analogWrite(MOTOR_A_SPEED, motorSpeed);
  analogWrite(MOTOR_B_SPEED, motorSpeed);
  Serial.println("Turning Left");
}

void moveForward() {
  digitalWrite(MOTOR_A1, HIGH);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, HIGH);
  analogWrite(MOTOR_A_SPEED, motorSpeed);
  analogWrite(MOTOR_B_SPEED, motorSpeed);
  Serial.println("Turning Right");
}

void stopMotors() {
  digitalWrite(MOTOR_A1, LOW);
  digitalWrite(MOTOR_A2, LOW);
  digitalWrite(MOTOR_B1, LOW);
  digitalWrite(MOTOR_B2, LOW);
  analogWrite(MOTOR_A_SPEED, 0);
  analogWrite(MOTOR_B_SPEED, 0);
  Serial.println("Motors Stopped");
}