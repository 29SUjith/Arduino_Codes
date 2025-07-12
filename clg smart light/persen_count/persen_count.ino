#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#define TRIG1 D1
#define ECHO1 D2
#define TRIG2 D5
#define ECHO2 D6
#define RELAY D7

const int threshold = 50;
const char* ssid = "Project";
const char* password = "12345678";

ESP8266WebServer server(80);

unsigned long detectionTime1 = 0;
unsigned long detectionTime2 = 0;
int personCount = 0;
bool lightState = false;  // false = OFF, true = ON

void setup() {
  Serial.begin(115200);

  pinMode(TRIG1, OUTPUT);
  pinMode(ECHO1, INPUT);
  pinMode(TRIG2, OUTPUT);
  pinMode(ECHO2, INPUT);
  pinMode(RELAY, OUTPUT);
  digitalWrite(RELAY, HIGH); // Light OFF initially

  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected. IP: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/toggle", handleToggle);
  server.on("/status", handleStatus);
  server.begin();
  Serial.println("Server started");
}

void loop() {
  server.handleClient();

  long dist1 = getDistance(TRIG1, ECHO1);
  long dist2 = getDistance(TRIG2, ECHO2);
  unsigned long now = millis();

  // Entry logic
  if (dist1 < threshold && (now - detectionTime1 > 1000)) {
    detectionTime1 = now;
    if (now - detectionTime2 < 1000) {
      personCount++;
      Serial.print("Person ENTERED. Count: ");
      Serial.println(personCount);
      turnLightOn();
    }
  }

  // Exit logic
  if (dist2 < threshold && (now - detectionTime2 > 1000)) {
    detectionTime2 = now;
    if (now - detectionTime1 < 1000) {
      if (personCount > 0) {
        personCount--;
        Serial.print("Person EXITED. Count: ");
        Serial.println(personCount);
      }
      if (personCount == 0) {
        turnLightOff();
      }
    }
  }

  delay(100);
}

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

void turnLightOn() {
  lightState = true;
  digitalWrite(RELAY, LOW); // Active LOW relay
}

void turnLightOff() {
  lightState = false;
  digitalWrite(RELAY, HIGH);
}

void handleRoot() {
  String html = "<html><head><title>Room Monitor</title>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  html += "<style>body{font-family:sans-serif;text-align:center;padding:20px;}button{padding:10px 20px;font-size:18px;}</style>";
  html += "<script>";
  html += "setInterval(function(){fetch('/status').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('count').innerText=d.count;";
  html += "document.getElementById('light').innerText=d.light;";
  html += "});},1000);";
  html += "</script></head><body>";
  html += "<h1>People Counter</h1>";
  html += "<p>Number of People Inside: <strong id='count'>...</strong></p>";
  html += "<p>Light State: <strong id='light'>...</strong></p>";
  html += "<form action='/toggle'><button>Toggle Light</button></form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleToggle() {
  if (lightState)
    turnLightOff();
  else
    turnLightOn();
  server.sendHeader("Location", "/");
  server.send(303); // Redirect to root
}

void handleStatus() {
  String json = "{";
  json += "\"count\":" + String(personCount) + ",";
  json += "\"light\":\"" + String(lightState ? "ON" : "OFF") + "\"";
  json += "}";
  server.send(200, "application/json", json);
}
