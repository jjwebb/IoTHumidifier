#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>

const char* ssid = "";
const char* password = "";
IPAddress local_IP(192,168,2,45);
IPAddress gateway(192,168,2,1);
IPAddress subnet(225,225,225,0);
IPAddress primaryDNS(8,8,8,8);
IPAddress secondaryDNS(8,8,4,4);

// void setup() {
//   // put your setup code here, to run once:
//   // pinMode(LED_BUILTIN, OUTPUT);
//   Serial.begin(115200);

//   if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
//     Serial.println("STA failed to configure!");
//   }
//   WiFi.begin(ssid, password);
//   Serial.print("Connecting to WiFi");

//   while(WiFi.status() != WL_CONNECTED) {
//     Serial.print(".");
//     delay(500);
//   }

//   Serial.println("\nConnected to WiFi!");
//   Serial.print("IP address: ");
//   Serial.print(WiFi.localIP());
// }

// void loop() {
//   if ((WiFi.status() == WL_CONNECTED)){
//     long rnd = random(1,10);
//     HTTPClient client;

//     client.begin("http://jsonplaceholder.typicode.com/comments?id=" + String(rnd));
//     int httpCode = client.GET();
//     if (httpCode > 0) {
//       String payload = client.getString();      
//       char json[500];
//       payload.replace(" ", "");
//       payload.replace("\n", "");
//       payload.trim();
//       payload.remove(0, 1);
//       payload.toCharArray(json, 500);
//       StaticJsonDocument<200> doc;
//       deserializeJson(doc, json);
//       int id = doc["id"];
      
//       Serial.println("id: " + String(id));
//       Serial.println("\nStatuscode: "+ String(httpCode));
//       Serial.println(payload);
//     }
//     else {
//       Serial.println("Error on HTTP request");
//     }
//   }
//   else
//     Serial.println("Connection lost");

//   // put your main code here, to run repeatedly:
//   delay(10000);
//   // Serial.println("Hello loop!");
//   // digitalWrite(LED_BUILTIN, HIGH);
//   // delay(1000);
//   // digitalWrite(LED_BUILTIN, LOW);
// }

WebServer server(80);

uint8_t LED1pin = LED_BUILTIN;
bool LED1status = LOW;

uint8_t LED2pin = 5;
bool LED2status = LOW;

void handle_OnConnect();
void handle_led1on();
void handle_led1off();
void handle_led2on();
void handle_led2off();
void handle_NotFound();
String SendHTML(uint8_t led1stat,uint8_t led2stat);

void handle_OnConnect() {
  LED1status = LOW;
  LED2status = LOW;
  Serial.println("GPIO4 Status: OFF | GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,LED2status)); 
}

void handle_led1on() {
  LED1status = HIGH;
  Serial.println("GPIO4 Status: ON");
  server.send(200, "text/html", SendHTML(true,LED2status)); 
}

void handle_led1off() {
  LED1status = LOW;
  Serial.println("GPIO4 Status: OFF");
  server.send(200, "text/html", SendHTML(false,LED2status)); 
}

void handle_led2on() {
  LED2status = HIGH;
  Serial.println("GPIO5 Status: ON");
  server.send(200, "text/html", SendHTML(LED1status,true)); 
}

void handle_led2off() {
  LED2status = LOW;
  Serial.println("GPIO5 Status: OFF");
  server.send(200, "text/html", SendHTML(LED1status,false)); 
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(uint8_t led1stat,uint8_t led2stat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>LED Control</title>\n";
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
  ptr +="<h1>ESP32 Web Server</h1>\n";
    ptr +="<h3>Using Station(STA) Mode</h3>\n";
  
   if(led1stat)
  {ptr +="<p>LED1 Status: ON</p><a class=\"button button-off\" href=\"/led1off\">OFF</a>\n";}
  else
  {ptr +="<p>LED1 Status: OFF</p><a class=\"button button-on\" href=\"/led1on\">ON</a>\n";}

  if(led2stat)
  {ptr +="<p>LED2 Status: ON</p><a class=\"button button-off\" href=\"/led2off\">OFF</a>\n";}
  else
  {ptr +="<p>LED2 Status: OFF</p><a class=\"button button-on\" href=\"/led2on\">ON</a>\n";}

  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

void setup() {
  Serial.begin(115200);
  delay(100);
  pinMode(LED1pin, OUTPUT);
  pinMode(LED2pin, OUTPUT);

   
  if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)){
    Serial.println("STA failed to configure!");
  }

  Serial.println("Connecting to ");
  Serial.println(ssid);

  //connect to your local wi-fi network
  WiFi.begin(ssid, password);

  //check wi-fi is connected to wi-fi network
  while (WiFi.status() != WL_CONNECTED) {
  delay(1000);
  Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected..!");
  Serial.print("Got IP: ");  Serial.println(WiFi.localIP());

  server.on("/", handle_OnConnect);
  server.on("/led1on", handle_led1on);
  server.on("/led1off", handle_led1off);
  server.on("/led2on", handle_led2on);
  server.on("/led2off", handle_led2off);
  server.onNotFound(handle_NotFound);

  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
  if(LED1status)
  {digitalWrite(LED1pin, HIGH);}
  else
  {digitalWrite(LED1pin, LOW);}
  
  if(LED2status)
  {digitalWrite(LED2pin, HIGH);}
  else
  {digitalWrite(LED2pin, LOW);}
}