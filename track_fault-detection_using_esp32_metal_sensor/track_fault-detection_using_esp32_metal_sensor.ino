#include <WiFi.h>
#include <WebServer.h>
#include <SoftwareSerial.h>
#include <TinyGPS++.h>

const char* ssid = "Projects";
const char* password = "12345678@";

#define IN1 13
#define IN2 12
#define IN3 14
#define IN4 27

#define METAL_SENSOR_1 5
#define METAL_SENSOR_2 18

// GPS Module pins
#define GPS_RX_PIN 16
#define GPS_TX_PIN 17

WebServer server(80);
TinyGPSPlus gps;
SoftwareSerial ss(GPS_RX_PIN, GPS_TX_PIN);

String getSensorStatus() {
  bool metal1 = (digitalRead(METAL_SENSOR_1) == LOW);  // Detected when LOW
  bool metal2 = (digitalRead(METAL_SENSOR_2) == LOW);  // Detected when LOW

  String status = "<p>Sensor 1: ";
  status += metal1 ? "<span style='color:green;'>Detected</span>" : "<span style='color:red;'>Not Detected</span>";
  status += "</p><p>Sensor 2: ";
  status += metal2 ? "<span style='color:green;'>Detected</span>" : "<span style='color:red;'>Not Detected</span>";
  status += "</p>";

  return status;
}

String getGPSStatus() {
  String status = "";
  
  if (gps.location.isValid()) {
    status += "<p><span style='color:green;'>📍 GPS Connected</span></p>";
    status += "<p>Latitude: " + String(gps.location.lat(), 6) + "°</p>";
    status += "<p>Longitude: " + String(gps.location.lng(), 6) + "°</p>";
    status += "<p>Altitude: " + String(gps.altitude.meters(), 2) + " m</p>";
    status += "<p>Speed: " + String(gps.speed.kmph(), 2) + " km/h</p>";
    status += "<p>Satellites: " + String(gps.satellites.value()) + "</p>";
    
    if (gps.date.isValid() && gps.time.isValid()) {
      status += "<p>Time: " + String(gps.time.hour()) + ":" + 
                String(gps.time.minute()) + ":" + String(gps.time.second()) + "</p>";
      status += "<p>Date: " + String(gps.date.day()) + "/" + 
                String(gps.date.month()) + "/" + String(gps.date.year()) + "</p>";
    }
  } else {
    status += "<p><span style='color:orange;'>📡 Searching for GPS...</span></p>";
    status += "<p>Satellites: " + String(gps.satellites.value()) + "</p>";
    if (gps.charsProcessed() < 10) {
      status += "<p><span style='color:red;'>Check GPS wiring</span></p>";
    }
  }
  
  return status;
}

String HTML = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>ESP32 Car Control</title>
  <style>
    * {
      margin: 0;
      padding: 0;
      box-sizing: border-box;
    }

    body {
      font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      display: flex;
      justify-content: center;
      align-items: center;
      color: white;
      padding: 20px;
    }

    .container {
      background: rgba(255, 255, 255, 0.1);
      backdrop-filter: blur(10px);
      border-radius: 20px;
      padding: 40px;
      box-shadow: 0 8px 32px rgba(31, 38, 135, 0.37);
      border: 1px solid rgba(255, 255, 255, 0.18);
      text-align: center;
      max-width: 500px;
      width: 90%;
    }

    h2 {
      margin-bottom: 30px;
      font-size: 2.2em;
      text-shadow: 2px 2px 4px rgba(0,0,0,0.3);
      background: linear-gradient(45deg, #fff, #e0e0e0);
      -webkit-background-clip: text;
      -webkit-text-fill-color: transparent;
      background-clip: text;
    }

    .control-panel {
      margin-bottom: 30px;
    }

    .button-container {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 15px;
    }

    button {
      width: 120px;
      height: 50px;
      border: none;
      border-radius: 25px;
      font-size: 16px;
      font-weight: bold;
      cursor: pointer;
      transition: all 0.3s ease;
      box-shadow: 0 4px 15px rgba(0, 0, 0, 0.2);
      position: relative;
      overflow: hidden;
    }

    button:before {
      content: '';
      position: absolute;
      top: 0;
      left: -100%;
      width: 100%;
      height: 100%;
      background: linear-gradient(90deg, transparent, rgba(255,255,255,0.2), transparent);
      transition: left 0.5s;
    }

    button:hover:before {
      left: 100%;
    }

    .forward {
      background: linear-gradient(45deg, #4CAF50, #45a049);
      color: white;
    }

    .forward:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(76, 175, 80, 0.4);
    }

    .stop {
      background: linear-gradient(45deg, #f44336, #da190b);
      color: white;
    }

    .stop:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(244, 67, 54, 0.4);
    }

    .back {
      background: linear-gradient(45deg, #2196F3, #0b7dda);
      color: white;
    }

    .back:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 20px rgba(33, 150, 243, 0.4);
    }

    button:active {
      transform: translateY(0);
      box-shadow: 0 2px 10px rgba(0, 0, 0, 0.2);
    }

    .sensor-panel, .gps-panel, .stream-panel {
      background: rgba(255, 255, 255, 0.1);
      border-radius: 15px;
      padding: 20px;
      border: 1px solid rgba(255, 255, 255, 0.2);
      margin-bottom: 20px;
    }

    .sensor-title, .gps-title, .stream-title {
      font-size: 1.4em;
      margin-bottom: 15px;
      color: #fff;
      text-shadow: 1px 1px 2px rgba(0,0,0,0.3);
    }

    #sensor, #gps {
      font-size: 14px;
      line-height: 1.6;
    }

    #sensor p, #gps p {
      margin: 8px 0;
      padding: 8px;
      border-radius: 8px;
      background: rgba(255, 255, 255, 0.1);
    }

    #sensor span, #gps span {
      font-weight: bold;
      text-shadow: 1px 1px 2px rgba(0,0,0,0.5);
    }

    .loading {
      display: inline-block;
      animation: pulse 1.5s ease-in-out infinite;
    }

    @keyframes pulse {
      0% { opacity: 1; }
      50% { opacity: 0.5; }
      100% { opacity: 1; }
    }

    .status-indicator {
      display: inline-block;
      width: 12px;
      height: 12px;
      border-radius: 50%;
      margin-right: 8px;
      animation: blink 2s ease-in-out infinite;
    }

    .detected {
      background-color: #4CAF50;
      box-shadow: 0 0 10px #4CAF50;
    }

    .not-detected {
      background-color: #f44336;
      box-shadow: 0 0 10px #f44336;
    }

    @keyframes blink {
      0%, 100% { opacity: 1; }
      50% { opacity: 0.3; }
    }

    .iframe-container {
      position: relative;
      width: 100%;
      height: 300px;
      border-radius: 10px;
      overflow: hidden;
      box-shadow: 0 4px 20px rgba(0, 0, 0, 0.3);
    }

    .iframe-container iframe {
      width: 150%;
      height: 150%;
      border: none;
      border-radius: 10px;
      transform: scale(0.67);
      transform-origin: top left;
    }

    @media (max-width: 480px) {
      .container {
        padding: 30px 20px;
      }
      
      h2 {
        font-size: 1.8em;
      }
      
      button {
        width: 100px;
        height: 45px;
        font-size: 14px;
      }
      
      #sensor, #gps {
        font-size: 12px;
      }
      
      .iframe-container {
        height: 250px;
      }
    }
  </style>
</head>
<body>
  <div class="container">
    <h2>🚗 ESP32 Car Control</h2>
    
    <div class="control-panel">
      <div class="button-container">
        <button class="forward" onclick="sendCommand('forward')">⬆️ Forward</button>
        <button class="stop" onclick="sendCommand('stop')">⏹️ Stop</button>
        <button class="back" onclick="sendCommand('back')">⬇️ Back</button>
      </div>
    </div>

    <div class="sensor-panel">
      <div class="sensor-title">🔍 Metal Sensors</div>
      <div id="sensor" class="loading">
        Loading sensor data...
      </div>
    </div>

    <div class="gps-panel">
      <div class="gps-title">🛰️ GPS Location</div>
      <div id="gps" class="loading">
        Loading GPS data...
      </div>
    </div>

    <div class="stream-panel">
      <div class="stream-title">📹 Live Stream</div>
      <div class="iframe-container">
        <iframe src="https://streamsync-7yp3.onrender.com/live/14544dad" 
                frameborder="0" 
                allowfullscreen>
        </iframe>
      </div>
    </div>
  </div>

  <script>
    function sendCommand(cmd) {
      fetch("/move?dir=" + cmd);
    }

    setInterval(() => {
      fetch("/sensors")
        .then(res => res.text())
        .then(data => {
          document.getElementById("sensor").innerHTML = data;
          document.getElementById("sensor").classList.remove("loading");
        })
        .catch(err => {
          document.getElementById("sensor").innerHTML = "Connection error";
          document.getElementById("sensor").classList.add("loading");
        });
    }, 1000);

    setInterval(() => {
      fetch("/gps")
        .then(res => res.text())
        .then(data => {
          document.getElementById("gps").innerHTML = data;
          document.getElementById("gps").classList.remove("loading");
        })
        .catch(err => {
          document.getElementById("gps").innerHTML = "Connection error";
          document.getElementById("gps").classList.add("loading");
        });
    }, 2000);
  </script>
</body>
</html>
)rawliteral";

void setup() {
  Serial.begin(115200);
  ss.begin(9600);  // GPS module baud rate

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);

  pinMode(METAL_SENSOR_1, INPUT);
  pinMode(METAL_SENSOR_2, INPUT);

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  server.on("/", []() {
    server.send(200, "text/html", HTML);
  });

  server.on("/move", []() {
    String dir = server.arg("dir");
    if (dir == "forward") {
      digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
      digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
    } else if (dir == "back") {
      digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
      digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
    } else { // stop
      digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
      digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
    }
    server.send(200, "text/plain", "OK");
  });

  server.on("/sensors", []() {
    server.send(200, "text/html", getSensorStatus());
  });

  server.on("/gps", []() {
    server.send(200, "text/html", getGPSStatus());
  });

  server.begin();
  Serial.println("Server started");
  Serial.println("GPS module initializing...");
}

void loop() {
  server.handleClient();
  
  // Read GPS data
  while (ss.available() > 0) {
    if (gps.encode(ss.read())) {
      // GPS data is automatically processed by TinyGPS++
    }
  }
}