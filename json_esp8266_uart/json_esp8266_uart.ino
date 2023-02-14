#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>
#include <Arduino.h>
#include <WebSocketsClient.h>
#include <SoftwareSerial.h>

#define Rx D5
#define Tx D6
SoftwareSerial mySerialEsp(Rx, Tx);  //Khởi tạo cổng serial mềm

#define relay D2

String chuyenDong = "";
String giaTriChuyenDong = "";
String anhSang = "";
// int giaTriAnhSang = map(anhSang.toInt(),0,1023,0,6000);

String controlRelay = "AUTO";

String postData = "";

WebSocketsClient webSocket;


/* Set these to your desired credentials. */

const char *ssid = "whyfine";  //ENTER YOUR WIFI SETTINGS
const char *password = "xu@nn@m1711";

// const char *ssid = "THUONGHUYEN";  //ENTER YOUR WIFI SETTINGS
// const char *password = "huyen21112001";

// const char *ssid = "Teyvat";  //ENTER YOUR WIFI SETTINGS
// const char *password = "12345678";


//=======================================================================
//                    Power on setup
//=======================================================================

void setup() {

  pinMode(D2, OUTPUT);
  digitalWrite(D2, LOW);

  delay(1000);
  Serial.begin(9600);
  mySerialEsp.begin(9600);

  /*-------------------------------------CONNECT WIFI-----------------------------------------*/
  WiFi.mode(WIFI_OFF);  //Prevents reconnection issue (taking too long to connect)
  delay(1000);
  WiFi.mode(WIFI_STA);  //This line hides the viewing of ESP as wifi hotspot

  WiFi.begin(ssid, password);  //Connect to your WiFi router
  Serial.println("");

  Serial.print("Connecting");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  //If connection successful show IP address in serial monitor
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());  //IP address assigned to your ESP
  /*-----------------------------------------------------------------------------------------*/

  webSocket.begin("192.168.43.141", 5000);
  // webSocket.begin("192.168.1.7", 5000);

  webSocket.onEvent(webSocketEvent);
}

//=======================================================================
//                    Main Program Loop
//=======================================================================
void loop() {
  webSocket.loop();
  Read_Uart_Json();
  if (controlRelay == "AUTO") {
    if (giaTriChuyenDong == "Yes" && anhSang.toInt() > 800) {
      digitalWrite(D2, HIGH);
    } else{
      digitalWrite(D2, LOW);
    }
  } 
  else {
    ManualControl();
  }


  postData = "{\"CD\":\"" + String(giaTriChuyenDong) + "\", \"AS\":\"" + String(anhSang) + "\", \"TT\":\"" + String(controlRelay) + "\"}";
  webSocket.sendTXT(postData);
  postData = "";
}

void webSocketEvent(WStype_t type, uint8_t *payload, size_t length) {

  Serial.println("type: " + type);
  Serial.printf("payload: %s\n", payload);
  controlRelay = String((char *)payload);

  Serial.println("length: " + String(length));
  Serial.println("controlRelay: " + controlRelay);

  mySerialEsp.print(String((char *)payload));
}

void ManualControl() {
  if (controlRelay == "ON") {
    digitalWrite(D2, HIGH);
  } else if (controlRelay == "OFF") {
    digitalWrite(D2, LOW);
  }
}
void Read_Uart_Json() {
  if (mySerialEsp.available()) {
    // receiveString = Serial.readString();

    StaticJsonBuffer<256> doc;

    String receiveString = mySerialEsp.readString();// lấy dữ liệu bên ESP gửi sang

    Serial.println("data" + receiveString);

    JsonObject &obj = doc.parseObject(receiveString);

    chuyenDong = obj["CD"].as<String>();

    if (chuyenDong == "0") {
      giaTriChuyenDong = "No";
    } else {
      giaTriChuyenDong = "Yes";
    }

    anhSang = obj["AS"].as<String>();
  }
  Serial.println("Chuyen Dong: " + chuyenDong);
  Serial.println("Gia tri chuyen dong: " + giaTriChuyenDong);
  Serial.println("Anh Sang: " + anhSang);
  // Serial.println("Gia tri anh sang: "+ giaTriAnhSang);
}