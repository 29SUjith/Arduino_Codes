#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Project";
const char* password = "12345678";

const int irPins[] = {12, 14, 27, 26};

WebServer server(80);

String generateHTML() {
  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>IR Sensor Status</title>
      <style>
        body { 
          font-family: Arial, sans-serif; 
          text-align: center; 
          background-color: #D2B48C; 
          display: flex; 
          justify-content: center; 
          align-items: center; 
          height: 100vh; 
          margin: 0;
        }
        .container { 
          display: flex; 
          justify-content: center; 
          gap: 20px; 
          flex-wrap: wrap; 
        }
        .box { 
          width: 150px; 
          height: 150px; 
          display: flex; 
          justify-content: center; 
          align-items: center; 
          border: 2px solid black; 
          border-radius: 10px; 
          font-size: 18px; 
          color: white; 
        }
      </style>
      <script>
        async function updateStatus() {
          const response = await fetch('/status');
          const statuses = await response.json();
          const boxes = document.querySelectorAll('.box');
          statuses.forEach((status, index) => {
            boxes[index].style.backgroundColor = status ? 'red' : 'green';
            boxes[index].innerText = status ? 'ON' : 'OFF';
          });
        }
        setInterval(updateStatus, 2000);
      </script>
    </head>
    <body>
      <div class="container">
        <div class="box" id="sensor1">Loading...</div>
        <div class="box" id="sensor2">Loading...</div>
        <div class="box" id="sensor3">Loading...</div>
        <div class="box" id="sensor4">Loading...</div>
      </div>
    </body>
    </html>
  )rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", generateHTML());
}

void handleStatus() {
  bool statuses[4];
  for (int i = 0; i < 4; i++) {
    statuses[i] = digitalRead(irPins[i]) == LOW; // LOW indicates IR is ON
  }
  String json = "[";
  for (int i = 0; i < 4; i++) {
    json += statuses[i] ? "true" : "false";
    if (i < 3) json += ",";
  }
  json += "]";
  server.send(200, "application/json", json);
}

void setup() {
  Serial.begin(115200);

  for (int i = 0; i < 4; i++) {
    pinMode(irPins[i], INPUT_PULLUP);
  }


  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Wi-Fi");
  Serial.println("IP address: " + WiFi.localIP().toString());

  server.on("/", handleRoot);
  server.on("/status", handleStatus);

  server.begin();
}

void loop() {
  server.handleClient();
}