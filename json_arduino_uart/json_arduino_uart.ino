#include <ArduinoJson.h>
#include <SoftwareSerial.h>

#define Rx 10
#define Tx 12
SoftwareSerial mySerial(Rx,Tx); //Khởi tạo cổng serial mềm

#define camBienChuyenDong 8
#define camBienAnhSang A0


// String inputString = "";
// bool stringComplete = false;


String ChuoiSendEsp = "";
long chuyenDong = 0;
long anhSang = 0;
long last = 0;

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);

  last = millis();
}

void loop() {
  if(millis() - last >= 1000){
    camBien();
    DataJson( String(chuyenDong), String(anhSang) );
    last = millis();
  }

}
void camBien(){
  chuyenDong = digitalRead(8);
  anhSang = analogRead(A0);
}

void DataJson(String chuyenDong, String anhSang){
  // {"CD":"giatrichuyendong" , "AS":"giatrianhsang"}
  ChuoiSendEsp ="{\"CD\":\"" + String(chuyenDong) + "\", \"AS\":\"" + String(anhSang) + "\"}";

  Serial.println(ChuoiSendEsp);
  
  mySerial.print(ChuoiSendEsp);
  
}
