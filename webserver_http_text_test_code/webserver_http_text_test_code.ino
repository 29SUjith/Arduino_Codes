#include <ESP8266WiFi.h>

// Replace with your network credentials
const char* ssid = "Projects";
const char* password = "12345678@";

WiFiServer server(80);  // HTTP server on port 80

void setup() {
  Serial.begin(115200);
  delay(10);

  // Connect to Wi-Fi
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  WiFi.begin(ssid, password);

  // Wait until connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void loop() {
  WiFiClient client = server.available();  // Check for incoming clients

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {
          // End of HTTP header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-type:text/html");
          client.println();

          // HTML content
          client.println("<!DOCTYPE html><html><body>");
          client.println("<h1>Hello from ESP8266!</h1>");
          client.println("</body></html>");

          break;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client Disconnected.");
  }
}
