#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>

/**
 * ESP32 IoT humidifier controller. Serves a webpage at its assigned IP
 * address and can be controlled by using the HTML page it serves,
 * or by accessing the /on, /off, or /toggle endpoints.
 */

const char* ssid = "";
const char* password = "";
IPAddress local_IP(192,168,2,45);
IPAddress gateway(192,168,2,1);
IPAddress subnet(225,225,225,0);
IPAddress primaryDNS(8,8,8,8);
IPAddress secondaryDNS(8,8,4,4);

WebServer server(80);

// ESP-WROOM-32 GPIO5
uint8_t humidifierRelayStatusPin = 5;

// Humidifier is wired to a normally closed (connected) relay switch.
// This means that when the relay signal is LOW, the humidifier is ON.
bool humidifierRelayStatus = LOW;

void handle_OnConnect();
void handle_toggle();
void handle_on();
void handle_off();
void handle_NotFound();
String SendHTML(uint8_t humStat);

void handle_OnConnect() {
  String status = "Webpage accessed! Humidifier Status: ";
  status +=(humidifierRelayStatus ? "OFF" : "ON");
  Serial.println(status);
  digitalWrite(humidifierRelayStatusPin, humidifierRelayStatus);
  server.send(200, "text/html", SendHTML(humidifierRelayStatus));
}

void handle_toggle() {
  humidifierRelayStatus = (humidifierRelayStatus == LOW ? HIGH : LOW);
  String status = "Humidifier Status: ";
  status +=(humidifierRelayStatus ? "OFF" : "ON");
  Serial.println(status);
  digitalWrite(humidifierRelayStatusPin, humidifierRelayStatus);
  server.send(200, "text/html", SendHTML(humidifierRelayStatus));
}

void handle_on() {
  humidifierRelayStatus = LOW; // With normally closed relay, LOW is ON
  Serial.println("Humidifier Status: ON");
  digitalWrite(humidifierRelayStatusPin, humidifierRelayStatus);
  server.send(200, "text/html", SendHTML(humidifierRelayStatus));
}

void handle_off() {
  humidifierRelayStatus = HIGH; // With normally closed relay, HIGH is OFF
  Serial.println("Humidifier Status: OFF");
  digitalWrite(humidifierRelayStatusPin, humidifierRelayStatus);
  server.send(200, "text/html", SendHTML(humidifierRelayStatus));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t humStat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>Humidifier Control</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px;} h1 {color: #444444;margin: 50px auto 30px;} h3 {color: #444444;margin-bottom: 50px;}\n";
  ptr +=".button {display: block;width: 80px;background-color: #3498db;border: none;color: white;padding: 13px 30px;text-decoration: none;font-size: 25px;margin: 0px auto 35px;cursor: pointer;border-radius: 4px;}\n";
  ptr +=".button-on {background-color: #3498db;}\n";
  ptr +=".button-on:active {background-color: #2980b9;}\n";
  ptr +=".button-off {background-color: #34495e;}\n";
  ptr +=".button-off:active {background-color: #2c3e50;}\n";
  ptr +="p {font-size: 14px;color: #888;margin-bottom: 10px;}\n";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<h1>ESP32 Humidifier Web Controller</h1>\n";
  ptr +="<h3>Access the /on endpoint to turn on, /off to turn off or /toggle to toggle on or off</h3>\n";

  if(!humStat) // Invert status here because LOW/FALSE = ON
  {ptr +="<p>Humidifier Status: ON</p><a class=\"button button-off\" href=\"/off\">OFF</a>\n";}
  else
  {ptr +="<p>Humidifier Status: OFF</p><a class=\"button button-on\" href=\"/on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(humidifierRelayStatusPin, OUTPUT);
  digitalWrite(humidifierRelayStatusPin, humidifierRelayStatus);
   
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
    Serial.println("STA failed to configure!");
  }

  Serial.println("Connecting to ");
  Serial.println(ssid);

  // Connect to WiFi network specified by ssid/password
  WiFi.begin(ssid, password);

  // Check WiFi connected successfully
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/toggle", handle_toggle);
  server.on("/on", handle_on);
  server.on("/off", handle_off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
}