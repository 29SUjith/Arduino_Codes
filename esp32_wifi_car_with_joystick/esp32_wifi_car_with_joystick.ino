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
      margin: 25px 0;
      font-size: 2.5rem;
      color: #333;
    }
    .container {
      max-width: 800px;
      margin: 20px auto;
      margin-top: 60px;
      padding: 20px;
      background: #fff;
      border-radius: 10px;
      box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);
    }
    .joystick-container {
      position: relative;
      width: 400px;
      height: 400px;
      margin: 20px auto;
      background-color: #ddd;
      border-radius: 50%;
      box-shadow: 0 4px 6px rgba(0, 0, 0, 0.2);
      margin-bottom: 45px;
    }
    .joystick-knob {
      position: absolute;
      width: 100px;
      height: 100px;
      background-color: #008000;
      border-radius: 50%;
      top: 50%;
      left: 50%;
      transform: translate(-50%, -50%);
      touch-action: none;
    }
    .speed_text {
      font-size: 24px;
    }
    input[type="range"] {
      width: 100%;
      margin: 20px 0;
      height: 15px; /* Increased height for thickness */
      -webkit-appearance: none;
      background: #ddd;
      border-radius: 10px;
    }

    input[type="range"]::-webkit-slider-thumb {
      -webkit-appearance: none;
      appearance: none;
      width: 40px;
      height: 40px;
      background: #008000; 	
      border-radius: 50%;
      cursor: pointer;
    }

    input[type="range"]::-moz-range-thumb {
      width: 25px;
      height: 25px;
      background: #007BFF;
      border-radius: 50%;
      cursor: pointer;
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
    <div class="joystick-container" id="joystick">
      <div class="joystick-knob" id="knob"></div>
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
    const knob = document.getElementById('knob');
    const joystick = document.getElementById('joystick');
    const joystickRadius = joystick.offsetWidth / 2;
    const knobRadius = knob.offsetWidth / 2;

    joystick.addEventListener('pointerdown', startDrag);
    joystick.addEventListener('pointermove', drag);
    joystick.addEventListener('pointerup', stopDrag);
    joystick.addEventListener('pointerleave', stopDrag);

    let isDragging = false;
    let lastDirection = 'S'; // Default to stop when not moving

    function startDrag(event) {
      isDragging = true;
      moveKnob(event);
    }

    function drag(event) {
      if (isDragging) {
        moveKnob(event);
      }
    }

    function stopDrag() {
      isDragging = false;
      knob.style.transform = 'translate(-50%, -50%)'; // Reset position to center
      if (lastDirection !== 'S') { // Send stop only if last direction wasn't stop
        sendCommand('S');
      }
    }

    function moveKnob(event) {
      const rect = joystick.getBoundingClientRect();
      let offsetX = event.clientX - rect.left - joystickRadius;
      let offsetY = event.clientY - rect.top - joystickRadius;

      const distance = Math.sqrt(offsetX ** 2 + offsetY ** 2);
      if (distance > joystickRadius - knobRadius) {
        const angle = Math.atan2(offsetY, offsetX);
        offsetX = Math.cos(angle) * (joystickRadius - knobRadius);
        offsetY = Math.sin(angle) * (joystickRadius - knobRadius);
      }

      knob.style.transform = `translate(${offsetX - knobRadius}px, ${offsetY - knobRadius}px)`;

      const direction = getDirection(offsetX, offsetY);
      if (direction !== lastDirection) {
        lastDirection = direction;
        sendCommand(direction);
      }
    }

    function getDirection(x, y) {
      const angle = Math.atan2(y, x) * 180 / Math.PI;
      
      if (angle >= -45 && angle < 45) {
        return 'R'; // Right
      } else if (angle >= 45 && angle < 135) {
        return 'B'; // Backward
      } else if (angle >= -135 && angle < -45) {
        return 'F'; // Forward
      } else {
        return 'L'; // Left
      }
    }

    function sendCommand(command) {
      console.log(`Command: ${command}`); // For debugging purposes
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
  else stopCar();
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
