#include <WiFi.h>
#include <WebServer.h>

// Pin definitions
#define TRIG1 15
#define ECHO1 4
#define TRIG2 5
#define ECHO2 18
#define RELAY 23

// Wi-Fi credentials
const char* ssid = "Project";
const char* password = "12345678";

WebServer server(80);

// System variables
int threshold1 = 50;  // cm
int threshold2 = 50;  // cm
bool lightState = false;
int personCount = 0;

unsigned long detectionTime1 = 0;
unsigned long detectionTime2 = 0;

long sensor1Dist = 0;
long sensor2Dist = 0;

// Get distance from ultrasonic sensor
long getDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  long duration = pulseIn(echoPin, HIGH, 30000);
  long distance = duration * 0.034 / 2;
  return (duration == 0) ? 999 : distance;
}

// Handle root page
void handleRoot() {
  String html = R"rawliteral(
    <!DOCTYPE html><html>
    <head>
      <meta charset="utf-8">
      <title>Room Monitor</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
      <h2>Room Monitor</h2>
      <p>People Inside: <strong id="count">--</strong></p>
      <p>Light: <strong id="light">--</strong></p>
      <h3>Sensor Readings</h3>
      <p>Sensor 1: <strong id="s1">--</strong></p>
      <p>Sensor 2: <strong id="s2">--</strong></p>
      <h3>Thresholds</h3>
      <p>Sensor 1: <input type="number" id="th1" value="50"> cm</p>
      <p>Sensor 2: <input type="number" id="th2" value="50"> cm</p>
      <button onclick="setThresholds()">Set Thresholds</button>
      <br><br>
      <button onclick="toggleLight()">Toggle Light</button>

      <script>
        function setThresholds() {
          const t1 = document.getElementById('th1').value;
          const t2 = document.getElementById('th2').value;
          fetch(`/set?th1=${t1}&th2=${t2}`);
        }

        function toggleLight() {
          fetch('/toggle');
        }

        setInterval(function() {
          fetch('/status')
            .then(r => r.json())
            .then(d => {
              document.getElementById('count').innerText = d.count;
              document.getElementById('light').innerText = d.light;
              document.getElementById('th1').value = d.th1;
              document.getElementById('th2').value = d.th2;
              document.getElementById('s1').innerText = d.s1 + " cm";
              document.getElementById('s2').innerText = d.s2 + " cm";
            });
        }, 1000);
      </script>
    </body>
    </html>
  )rawliteral";
  server.send(200, "text/html", html);
}

// Return status as JSON
void handleStatus() {
  String json = "{";
  json += "\"count\":" + String(personCount) + ",";
  json += "\"light\":\"" + String(lightState ? "ON" : "OFF") + "\",";
  json += "\"th1\":" + String(threshold1) + ",";
  json += "\"th2\":" + String(threshold2) + ",";
  json += "\"s1\":" + String(sensor1Dist) + ",";
  json += "\"s2\":" + String(sensor2Dist);
  json += "}";
  server.send(200, "application/json", json);
}

// Set new thresholds
void handleSet() {
  if (server.hasArg("th1")) threshold1 = server.arg("th1").toInt();
  if (server.hasArg("th2")) threshold2 = server.arg("th2").toInt();
  server.send(200, "text/plain", "OK");
}

// Toggle relay light
void handleToggle() {
  lightState = !lightState;
  digitalWrite(RELAY, lightState ? LOW : HIGH);
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);

  // Sensor pins
  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(RELAY, OUTPUT);

  digitalWrite(RELAY, HIGH);  // Light OFF initially

  // Wi-Fi setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: " + WiFi.localIP().toString());

  // Routes
  server.on("/", handleRoot);
  server.on("/status", handleStatus);
  server.on("/set", handleSet);
  server.on("/toggle", handleToggle);
  server.begin();
}

void loop() {
  server.handleClient();

  long dist1 = getDistance(TRIG1, ECHO1);
  long dist2 = getDistance(TRIG2, ECHO2);
  unsigned long now = millis();

  sensor1Dist = dist1;
  sensor2Dist = dist2;

  // Enter logic
  if (dist1 < threshold1 && now - detectionTime1 > 1000) {
    detectionTime1 = now;
    if (now - detectionTime2 < 1000) {
      personCount++;
      Serial.print("Person ENTERED. Count: ");
      Serial.println(personCount);
      digitalWrite(RELAY, LOW);  // Turn ON light
      lightState = true;
    }
  }

  // Exit logic
  if (dist2 < threshold2 && now - detectionTime2 > 1000) {
    detectionTime2 = now;
    if (now - detectionTime1 < 1000) {
      if (personCount > 0) {
        personCount--;
        Serial.print("Person EXITED. Count: ");
        Serial.println(personCount);
        if (personCount == 0) {
          digitalWrite(RELAY, HIGH);  // Turn OFF light
          lightState = false;
        }
      }
    }
  }

  delay(100);
}
