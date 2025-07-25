#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Projects";        // Your WiFi SSID
const char* password = "12345678@";   // Your WiFi Password

#define WATER_SENSOR_PIN 34     // Water level sensor analog pin
#define PUMP_RELAY_PIN 15       // Pump relay control pin

WebServer server(80);

bool pumpState = false;       // Current pump state
bool manualOverride = false;  // Manual control flag

// Function to update pump based on water detection (if not manually overridden)
void updatePump() {
  if (!manualOverride) {
    int waterLevel = analogRead(WATER_SENSOR_PIN);
    // When water is detected (lower values indicate water presence)
    if (waterLevel < 3000) {  // Adjust threshold based on your sensor (water detected < 3000)
      pumpState = true;
    } else {
      pumpState = false;
    }
    digitalWrite(PUMP_RELAY_PIN, pumpState ? LOW : HIGH);  // Active LOW relay
  }
}

// Smart Underpass webpage content
String getHTML() {
  String pumpStatus = pumpState ? "ACTIVE" : "STANDBY";
  int waterLevel = analogRead(WATER_SENSOR_PIN);
  String waterStatus = (waterLevel < 3000) ? "DETECTED" : "CLEAR";
  String modeStatus = manualOverride ? "Manual Override" : "Auto Drainage";
  String alertLevel = "NORMAL";
  
  // Determine alert level based on water reading (lower values = more water)
  if (waterLevel < 1000) {
    alertLevel = "CRITICAL";
  } else if (waterLevel < 1500) {
    alertLevel = "HIGH";
  } else if (waterLevel < 3000) {
    alertLevel = "MODERATE";
  }
  
  // Calculate water level percentage (lower reading = higher water level)
  int waterPercent = map(waterLevel, 4095, 0, 0, 100);
  waterPercent = constrain(waterPercent, 0, 100);
  
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Underpass Management</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #1e3c72 0%, #2a5298 100%);
            min-height: 100vh;
            padding: 15px;
        }
        
        .container {
            max-width: 800px;
            margin: 0 auto;
            background: rgba(255, 255, 255, 0.98);
            border-radius: 20px;
            box-shadow: 0 25px 50px rgba(0,0,0,0.15);
            padding: 30px;
            backdrop-filter: blur(10px);
        }
        
        .header {
            text-align: center;
            margin-bottom: 30px;
            border-bottom: 3px solid #f1f2f6;
            padding-bottom: 20px;
        }
        
        .header h1 {
            color: #2c3e50;
            font-size: 2.5em;
            margin-bottom: 10px;
            font-weight: 300;
        }
        
        .header p {
            color: #7f8c8d;
            font-size: 1.2em;
        }
        
        .alert-banner {
            padding: 15px 20px;
            border-radius: 12px;
            text-align: center;
            font-weight: bold;
            margin-bottom: 25px;
            font-size: 1.1em;
        }
        
        .alert-normal {
            background: linear-gradient(135deg, #d4edda, #c3e6cb);
            color: #155724;
            border: 2px solid #c3e6cb;
        }
        
        .alert-moderate {
            background: linear-gradient(135deg, #fff3cd, #ffeaa7);
            color: #856404;
            border: 2px solid #ffeaa7;
        }
        
        .alert-high {
            background: linear-gradient(135deg, #f8d7da, #f5c6cb);
            color: #721c24;
            border: 2px solid #f5c6cb;
        }
        
        .alert-critical {
            background: linear-gradient(135deg, #dc3545, #c82333);
            color: white;
            border: 2px solid #c82333;
            animation: pulse 2s infinite;
        }
        
        @keyframes pulse {
            0% { transform: scale(1); }
            50% { transform: scale(1.02); }
            100% { transform: scale(1); }
        }
        
        .status-grid {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .status-card {
            background: #fff;
            border-radius: 15px;
            padding: 25px;
            text-align: center;
            box-shadow: 0 8px 20px rgba(0,0,0,0.1);
            border: 2px solid #f1f2f6;
            transition: all 0.3s ease;
            position: relative;
            overflow: hidden;
        }
        
        .status-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 15px 35px rgba(0,0,0,0.15);
        }
        
        .status-card h3 {
            color: #2c3e50;
            font-size: 1.1em;
            margin-bottom: 15px;
            font-weight: 600;
        }
        
        .status-value {
            font-size: 1.8em;
            font-weight: bold;
            margin-bottom: 10px;
        }
        
        .water-status {
            color: #3498db;
        }
        
        .water-status.detected {
            color: #e74c3c;
        }
        
        .pump-status {
            color: #95a5a6;
        }
        
        .pump-status.active {
            color: #27ae60;
        }
        
        .water-level-container {
            background: #ecf0f1;
            border-radius: 10px;
            height: 25px;
            margin: 15px 0;
            overflow: hidden;
            position: relative;
        }
        
        .water-level-bar {
            height: 100%;
            border-radius: 10px;
            transition: all 0.5s ease;
            position: relative;
        }
        
        .level-normal { background: linear-gradient(90deg, #27ae60, #2ecc71); }
        .level-moderate { background: linear-gradient(90deg, #f39c12, #e67e22); }
        .level-high { background: linear-gradient(90deg, #e74c3c, #c0392b); }
        .level-critical { 
            background: linear-gradient(90deg, #8e44ad, #9b59b6);
            animation: flash 1s infinite;
        }
        
        @keyframes flash {
            0%, 50% { opacity: 1; }
            25%, 75% { opacity: 0.7; }
        }
        
        .controls {
            display: grid;
            grid-template-columns: 1fr 1fr 1fr;
            gap: 15px;
            margin-top: 30px;
        }
        
        .btn {
            padding: 18px 25px;
            border: none;
            border-radius: 12px;
            font-size: 1.1em;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            text-decoration: none;
            display: inline-block;
            text-align: center;
            color: white;
            position: relative;
            overflow: hidden;
        }
        
        .btn:hover {
            transform: translateY(-3px);
            box-shadow: 0 10px 25px rgba(0,0,0,0.2);
        }
        
        .btn-start {
            background: linear-gradient(135deg, #27ae60, #2ecc71);
        }
        
        .btn-stop {
            background: linear-gradient(135deg, #e74c3c, #c0392b);
        }
        
        .btn-auto {
            background: linear-gradient(135deg, #3498db, #2980b9);
        }
        
        .mode-indicator {
            text-align: center;
            margin: 20px 0;
            padding: 18px;
            border-radius: 12px;
            font-weight: 600;
            font-size: 1.1em;
        }
        
        .mode-auto {
            background: linear-gradient(135deg, #d4edda, #c3e6cb);
            color: #155724;
            border: 2px solid #c3e6cb;
        }
        
        .mode-manual {
            background: linear-gradient(135deg, #fff3cd, #ffeaa7);
            color: #856404;
            border: 2px solid #ffeaa7;
        }
        
        .info-text {
            color: #7f8c8d;
            font-size: 0.95em;
            margin-top: 8px;
        }
        
        .system-info {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 15px;
            margin-top: 25px;
        }
        
        .info-box {
            background: rgba(108, 117, 125, 0.1);
            padding: 15px;
            border-radius: 10px;
            text-align: center;
        }
        
        .info-box h4 {
            color: #495057;
            margin-bottom: 8px;
        }
        
        .info-box p {
            color: #6c757d;
            font-size: 0.9em;
        }
        
        @media (max-width: 768px) {
            .status-grid {
                grid-template-columns: 1fr;
            }
            
            .controls {
                grid-template-columns: 1fr;
            }
            
            .system-info {
                grid-template-columns: 1fr;
            }
            
            .container {
                margin: 10px;
                padding: 20px;
            }
            
            .header h1 {
                font-size: 2em;
            }
        }
        
        .refresh-indicator {
            position: fixed;
            top: 20px;
            right: 20px;
            background: rgba(52, 152, 219, 0.95);
            color: white;
            padding: 12px 18px;
            border-radius: 25px;
            font-size: 0.9em;
            opacity: 0;
            transition: opacity 0.3s ease;
            z-index: 1000;
        }
    </style>
    <script>
        // Auto refresh every 3 seconds for critical monitoring
        setTimeout(function() {
            location.reload();
        }, 3000);
        
        // Show refresh indicator
        window.onload = function() {
            const indicator = document.querySelector('.refresh-indicator');
            indicator.style.opacity = '1';
            setTimeout(() => {
                indicator.style.opacity = '0';
            }, 2000);
        }
    </script>
</head>
<body>
    <div class="refresh-indicator">🔄 System Monitoring...</div>
    
    <div class="container">
        <div class="header">
            <h1>🌉 Smart Underpass Control</h1>
            <p>Automated Water Detection & Drainage System</p>
        </div>
        
        <div class="alert-banner alert-)rawliteral";
        
  if (alertLevel == "CRITICAL") html += "critical";
  else if (alertLevel == "HIGH") html += "high";
  else if (alertLevel == "MODERATE") html += "moderate";
  else html += "normal";
  
  html += R"rawliteral(">
            🚨 ALERT LEVEL: )rawliteral";
  html += alertLevel;
  html += R"rawliteral( - )rawliteral";
  
  if (alertLevel == "CRITICAL") html += "IMMEDIATE DRAINAGE REQUIRED";
  else if (alertLevel == "HIGH") html += "HIGH WATER LEVEL DETECTED";
  else if (alertLevel == "MODERATE") html += "WATER DETECTED - MONITORING";
  else html += "UNDERPASS CLEAR - NORMAL OPERATIONS";
  
  html += R"rawliteral(
        </div>
        
        <div class="mode-indicator )rawliteral";
  html += manualOverride ? "mode-manual" : "mode-auto";
  html += R"rawliteral(">
            🔧 Current Mode: )rawliteral";
  html += modeStatus;
  html += R"rawliteral(
        </div>
        
        <div class="status-grid">
            <div class="status-card">
                <h3>💧 Water Detection</h3>
                <div class="status-value water-status )rawliteral";
  html += (waterLevel < 3000) ? "detected" : "";
  html += R"rawliteral(">)rawliteral";
  html += waterStatus;
  html += R"rawliteral(</div>
                <div class="water-level-container">
                    <div class="water-level-bar )rawliteral";
  
  if (alertLevel == "CRITICAL") html += "level-critical";
  else if (alertLevel == "HIGH") html += "level-high";
  else if (alertLevel == "MODERATE") html += "level-moderate";
  else html += "level-normal";
  
  html += R"rawliteral(" style="width: )rawliteral";
  html += String(waterPercent);
  html += R"rawliteral(%;"></div>
                </div>
                <div class="info-text">Level: )rawliteral";
  html += String(waterLevel) + " (" + String(waterPercent) + "%)";
  html += R"rawliteral(</div>
            </div>
            
            <div class="status-card">
                <h3>⚡ Drainage Pump</h3>
                <div class="status-value pump-status )rawliteral";
  html += pumpState ? "active" : "";
  html += R"rawliteral(">)rawliteral";
  html += pumpStatus;
  html += R"rawliteral(</div>
                <div class="info-text">)rawliteral";
  html += pumpState ? "🔄 Actively draining water" : "⏸️ Ready for activation";
  html += R"rawliteral(</div>
            </div>
            
            <div class="status-card">
                <h3>📊 System Status</h3>
                <div class="status-value" style="color: )rawliteral";
  html += (alertLevel == "NORMAL") ? "#27ae60" : "#e74c3c";
  html += R"rawliteral(;">)rawliteral";
  html += (alertLevel == "NORMAL") ? "OPERATIONAL" : "ALERT";
  html += R"rawliteral(</div>
                <div class="info-text">)rawliteral";
  html += manualOverride ? "Manual control active" : "Auto monitoring active";
  html += R"rawliteral(</div>
            </div>
        </div>
        
        <div class="controls">
            <a href="/manual/on" class="btn btn-start">
                🚿 Start Pump
            </a>
            <a href="/manual/off" class="btn btn-stop">
                ⏹️ Stop Pump
            </a>
            <a href="/auto" class="btn btn-auto">
                🤖 Auto Mode
            </a>
        </div>
        
        <div class="system-info">
            <div class="info-box">
                <h4>🌐 System IP</h4>
                <p>)rawliteral";
  html += WiFi.localIP().toString();
  html += R"rawliteral(</p>
            </div>
            <div class="info-box">
                <h4>⏱️ Update Rate</h4>
                <p>Real-time (3 sec refresh)</p>
            </div>
        </div>
    </div>
</body>
</html>
)rawliteral";

  return html;
}

// Web handlers
void handleRoot() {
  updatePump();
  server.send(200, "text/html", getHTML());
}

void handleManualOn() {
  manualOverride = true;
  pumpState = true;
  digitalWrite(PUMP_RELAY_PIN, LOW); // Active LOW relay
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleManualOff() {
  manualOverride = true;
  pumpState = false;
  digitalWrite(PUMP_RELAY_PIN, HIGH); // Active LOW relay
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleAuto() {
  manualOverride = false;
  updatePump();
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);
  pinMode(WATER_SENSOR_PIN, INPUT);
  pinMode(PUMP_RELAY_PIN, OUTPUT);
  digitalWrite(PUMP_RELAY_PIN, HIGH); // Turn off pump initially (active low)

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/manual/on", handleManualOn);
  server.on("/manual/off", handleManualOff);
  server.on("/auto", handleAuto);
  server.begin();
  Serial.println("Underpass Management Server started");
}

void loop() {
  server.handleClient();
  if (!manualOverride) {
    updatePump();  // Keep auto drainage running
  }
  delay(1500);  // Update every 1.5 seconds for critical monitoring
}