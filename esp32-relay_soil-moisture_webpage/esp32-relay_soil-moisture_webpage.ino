#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Project";
const char* password = "12345678";

WebServer server(80);

const int soilPin = 34;
const int relayPin = 26;

bool motorState = false;
bool autoMode = true;
int threshold = 30;

void setup() {
  Serial.begin(115200);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, HIGH); // OFF

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/data", handleData);            // AJAX route
  server.on("/toggle", toggleMotor);
  server.on("/setThreshold", handleSetThreshold);

  server.begin();
}

void loop() {
  server.handleClient();

  if (autoMode) {
    int moisture = analogRead(soilPin);
    int percentage = map(moisture, 4095, 0, 0, 100);

    if (percentage < threshold) {
      digitalWrite(relayPin, LOW);
      motorState = true;
    } else {
      digitalWrite(relayPin, HIGH);
      motorState = false;
    }
  }
}

void handleRoot() {
  String html = R"rawliteral(
<!DOCTYPE html><html><head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>Smart Irrigation</title>
  <style>
    body {
      font-family: 'Segoe UI', sans-serif;
      margin: 0;
      background: linear-gradient(to bottom, #e6f9f0, #ccf2ff);
    }
    .container {
      max-width: 400px;
      margin: auto;
      padding: 20px;
    }
    .card {
      background-color: white;
      border-radius: 15px;
      padding: 20px;
      box-shadow: 0 8px 16px rgba(0,0,0,0.15);
    }
    h2 {
      color: #2e7d32;
      margin-bottom: 10px;
    }
    .status {
      font-size: 18px;
      margin: 10px 0;
    }
    .status span {
      font-weight: bold;
    }
    .moisture { color: #00796b; }
    .motor { color: #0d47a1; }
    .threshold { color: #6d4c41; }
    button, input[type=submit] {
      background-color: #43a047;
      color: white;
      border: none;
      padding: 10px 20px;
      font-size: 16px;
      margin: 10px 0;
      border-radius: 8px;
      cursor: pointer;
    }
    button:hover, input[type=submit]:hover {
      background-color: #388e3c;
    }
    input[type=number] {
      width: 60px;
      padding: 6px;
      font-size: 16px;
      border: 1px solid #ccc;
      border-radius: 5px;
      margin-top: 10px;
    }
    .icon {
      font-size: 30px;
      display: inline-block;
      margin-bottom: 5px;
    }
  </style>
  <script>
    function fetchData() {
      fetch("/data")
        .then(response => response.json())
        .then(data => {
          document.getElementById("moisture").innerText = data.moisture + "%";
          document.getElementById("motor").innerText = data.motor ? "ON" : "OFF";
          document.getElementById("threshold").innerText = data.threshold + "%";
        });
    }
    setInterval(fetchData, 2000);
    window.onload = fetchData;
  </script>
</head><body>
  <div class="container">
    <div class="card">
      <h2>Smart Irrigation Dashboard</h2>
      <div class="status moisture">Moisture: <span id="moisture">--%</span></div>
      <div class="status motor">Motor: <span id="motor">--</span></div>
      <div class="status threshold">Threshold: <span id="threshold">--%</span></div>

      <form action="/toggle" method="GET">
        <button type="submit">Toggle Motor</button>
      </form>

      <form action="/setThreshold" method="GET">
        <label>Set Threshold (%):</label><br>
        <input type="number" name="th" min="0" max="100">
        <input type="submit" value="Set">
      </form>
    </div>
  </div>
</body></html>
)rawliteral";

  server.send(200, "text/html", html);
}


void handleData() {
  int moisture = analogRead(soilPin);
  int percentage = map(moisture, 4095, 0, 0, 100);

  String json = "{";
  json += "\"moisture\":" + String(percentage) + ",";
  json += "\"motor\":" + String(motorState ? "true" : "false") + ",";
  json += "\"threshold\":" + String(threshold);
  json += "}";

  server.send(200, "application/json", json);
}

void toggleMotor() {
  autoMode = false;
  motorState = !motorState;
  digitalWrite(relayPin, motorState ? LOW : HIGH);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleSetThreshold() {
  if (server.hasArg("th")) {
    threshold = server.arg("th").toInt();
    autoMode = true;
  }
  server.sendHeader("Location", "/");
  server.send(303);
}
