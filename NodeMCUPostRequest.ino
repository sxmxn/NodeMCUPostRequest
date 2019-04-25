#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <SDS011.h>
#include <SoftwareSerial.h>


#define SDS_RX D1
#define SDS_TX D2


const char* ssid     = "WLAN Name";
const char* password = "Password";
float p10,p25;
int error;
char server[] = "Server";
String name = "NodeMCUTomathy";

struct Air {
  float pm25;
  float pm10;
};


//ESP8266WebServer server(80);
WiFiClient client;
SDS011 sds;

void setup() {
  Serial.begin(9600);
  sds.begin(SDS_RX,SDS_TX);
  connectToWiFi();
}

void loop() {
  Air airData = readPolution();
  Serial.print("PM25: ");
  Serial.println(String(airData.pm25));
  Serial.print("PM10: ");
  Serial.println(String(airData.pm10));
  if (client.connect(server, 80)) {
    Serial.println("connected to server");
    // Make a HTTP request:

    String PostData="pm2_5=";
    PostData=String(PostData + airData.pm25);
    PostData=PostData+"&pm10=";
    PostData=String(PostData + airData.pm10);
    PostData=PostData+"&name=";
    PostData=String(PostData+name);
    Serial.println(PostData);
    
    client.println("POST /API/XXX HTTP/1.1");
    client.println("Host: HOST");
    client.println("Cache-Control: no-cache");
    client.println("Content-Type: application/x-www-form-urlencoded");
    client.print("Content-Length: ");
    client.println(PostData.length());
    client.println();
    client.println(PostData);
    client.println("Connection: close");
    client.println();
  }
  delay(600000);
}

void connectToWiFi(){
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
 
  Serial.print("Connecting to ");
  Serial.println(ssid); 

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

Air readPolution(){
  error = sds.read(&p25,&p10);
  if (!error) { 
    Air result = (Air){p25,p10};
    return result;
  } else {
    Serial.println("Error reading SDS011");
    return (Air){0.0, 0.0};
  }
}
