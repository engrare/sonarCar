#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "Jimmy 2,4 GHz";  // Your WiFi SSID with spaces
const char* password = "22184286756515604270";  // Replace with your WiFi password

ESP8266WebServer server(80);  // Create a server on port 80

String htmlPage = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
  <title>ESP8266 Web Server</title>
</head>
<body>
  <h2>Send Text to ESP8266</h2>
  <input type="text" id="inputText" placeholder="Enter text">
  <button onclick="sendText()">Send</button>
  <script>
    function sendText() {
      const inputText = document.getElementById("inputText").value;
      fetch("/send", {
        method: "POST",
        headers: {
          "Content-Type": "application/x-www-form-urlencoded"
        },
        body: "text=" + encodeURIComponent(inputText)
      }).then(response => response.text())
        .then(data => console.log(data))
        .catch(error => console.error("Error:", error));
    }
  </script>
</body>
</html>
)rawliteral";

void handleRoot() {
  server.send(200, "text/html", htmlPage);
}

void handleSendText() {
  if (server.method() == HTTP_POST) {
    String text = server.arg("text");  // Get the text from the POST request
    Serial.println("Received text: " + text);
    server.send(200, "text/plain", "Text received");
  } else {
    server.send(405, "text/plain", "Method Not Allowed");
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nConnected to WiFi");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, handleRoot);
  server.on("/send", HTTP_POST, handleSendText);
  server.begin();
}

void loop() {
  server.handleClient();
}
