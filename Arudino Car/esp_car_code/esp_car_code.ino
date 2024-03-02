#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <ESP8266WebServer.h>

#include <FS.h>

ESP8266WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

void handleRoot() {
  Serial.println("Handling root");
  File file = SPIFFS.open("/index.html", "r");
  if(!file){
    Serial.println("File not found");
    return;
  }
  server.streamFile(file, "text/html");
  //file.close();
}

void handleNotFound() {
  Serial.println("Handling not found");
  // Existing code...
}
String lastReceived = "";

void readAndSendSerialData() {
  if (Serial.available() > 0) {
    // Read the incoming data from serial
    String incomingData = Serial.readStringUntil('\n'); // Read until newline character

    // Optional: Trim whitespace and newline characters
    incomingData.trim();

    // Check if the data is not empty and contains a comma
    if (incomingData.length() > 0 && incomingData.indexOf(',') != -1) {
      // Send the data over WebSocket to all connected clients
      webSocket.broadcastTXT(incomingData);

      // Update the last received message
      lastReceived = incomingData;
    }
  }
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", num);
      break;
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(num);
        Serial.printf("[%u] Connected from %d.%d.%d.%d\n", num, ip[0], ip[1], ip[2], ip[3]);
      }
      break;
    case WStype_TEXT:
       Serial.printf("%sx", payload);
      // Echo text back to client
      webSocket.sendTXT(num, payload, length);
      break;
  }
}

void setup() {
  Serial.begin(9600);
  WiFi.begin("SOURCE", "Pelle!23");
  WiFi.setSleepMode(WIFI_NONE_SLEEP);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  
  // Initialize SPIFFS
  SPIFFS.begin();

  IPAddress ipAddress = WiFi.localIP();
  Serial.println(ipAddress); // Simplified way to print IP address
  
  server.on("/", handleRoot);
  server.serveStatic("/", SPIFFS, "/");
  server.onNotFound(handleNotFound);
  
  server.begin();
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);
}
void loop() {
  server.handleClient();
  webSocket.loop();
  readAndSendSerialData();

}
