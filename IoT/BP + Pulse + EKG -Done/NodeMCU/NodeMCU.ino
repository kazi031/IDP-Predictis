#include<SoftwareSerial.h>
#include <FirebaseArduino.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFi.h>

#define FIREBASE_HOST "demopredictis-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "CymjhVl2k3WCyptWAizU6tLT6EFUw5GiNqk7DnEp" 
#define ON_Board_LED 2


SoftwareSerial SUART(4, 5);  //D2, D1 = SRX, STX

//SoftwareSerial mySerial(D1,D2); //Rx,Tx pin
//SoftwareSerial SUART(4, 5);  //D2, D1 = SRX, STX

const char* ssid = "ABRAR"; 
const char* password = "01842890830";

int yy;

int thalach = 0;

int ecg = 0;

void setup() {
  Serial.begin(115200);
  SUART.begin(115200);
  WiFi.begin(ssid, password); 
  Serial.println("");

  delay(500);  
  pinMode(ON_Board_LED,OUTPUT); 
  digitalWrite(ON_Board_LED, HIGH); 
  Serial.print("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    digitalWrite(ON_Board_LED, LOW);
    delay(250);
    digitalWrite(ON_Board_LED, HIGH);
    delay(250);
  }
  digitalWrite(ON_Board_LED, HIGH);
  Serial.println("");
  Serial.print("Successfully connected to : ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  while(!Serial){
    ;
  }
  pinMode(D5, INPUT);
  pinMode(D6, INPUT);
}

void loop() {
  
  Serial.println(yy);
  int x = Firebase.getInt("Status/BP");
  int y = Firebase.getInt("Status/PULSE");
  int z = Firebase.getInt("Status/ECG");
  StaticJsonBuffer<1000> jsonBuffer;
  delay(2000);
  Serial.print("Status : ");
  if(x == 1){
    yy = yy + 1;
    Serial.println("Blood Pressure On");
    //Serial.println(x);
    JsonObject& S = jsonBuffer.createObject();
    S["status"] = 11;
    S.printTo(SUART);
    int xx = 0;
    if(yy == 15){
      while(xx == 0){
        if(Serial.available()){
          JsonObject& bp = jsonBuffer.parseObject(Serial);
          if(bp == JsonObject::invalid()){
            return;
          }
          xx = 1;
          Serial.print("SYS : ");
          int sys = int(bp["sys"]);
          Serial.println(sys);
          Serial.print("DIA : ");
          int dia = int(bp["dia"]);
          Serial.println(dia);
          Serial.print("BPM : ");
          int bpm = int(bp["bpm"]);
          Serial.println(bpm);
          StaticJsonBuffer<256> jsonBuffer;
          JsonArray& array = jsonBuffer.createArray();
          String SYS = String(sys);
          String DIA = String(dia);
          String BPM = String(bpm);
          array.add(SYS);
          array.add(DIA);
          array.add(BPM);
          Firebase.push("Blood_Pressure", array);
          yy = 0;
        }
      }
    }
  }
  else if(y == 1){
    yy = yy + 1;
    Serial.println("Pluse On");
    //Serial.println(y);
    JsonObject& S = jsonBuffer.createObject();
    S["status"] = 21;
    S.printTo(SUART);
    int jj = 0;
    if(yy == 10){
      while(jj != 10){
        if(Serial.available()){
          JsonObject& pulse = jsonBuffer.parseObject(Serial);
          if(pulse == JsonObject::invalid()){
            return;
          }
          jj = jj + 1;
          Serial.print("BPM COUNT : ");
          Serial.println(jj);
          Serial.print("PULSE : ");
          int hr = int(pulse["BPMval"]);
          if(hr > thalach){
            thalach = hr;
          }
          Serial.println(hr);
          String dataSend = String(hr);
          Firebase.pushString("PulseRate/Value",dataSend);
        }
      }
      Serial.print("Thalach: ");
      Serial.println(thalach);
      String dataSend1 = String(thalach);
      Firebase.pushString("Thalach/Value",dataSend1);
      thalach = 0;
      yy = 0;
    }
  }
  if(z == 1){
    yy = yy + 1;
    Serial.println("ECG On");
    int jj = 0;
    if(yy == 15){
      while(jj != 100){
        jj = jj + 1;
        if((digitalRead(D5) == 1)||(digitalRead(D6) == 1)){
        Serial.println("!");
        }
        else{
          ecg = analogRead(A0);
          ecg = ecg * 20;
          Serial.println(ecg);
          if((ecg >= 100) && (ecg <= 600)){
            String datasend = String(ecg);
            Serial.print("ECG/EKG data : ");
            Serial.println(datasend);
            Firebase.pushString("ECG/Value",datasend);
            if (Firebase.failed()){ 
              Serial.print("Setting /Value failed :");
              Serial.println(Firebase.error());  
              //delay(500);
              return;
            }
          delay(200);
          }
        }
      }
      yy = 0;
    }
  }
}
