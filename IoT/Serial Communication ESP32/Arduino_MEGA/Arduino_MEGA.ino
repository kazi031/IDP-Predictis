#include <ArduinoJson.h>

String str;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  //Serial.print(char(169));
  Serial.println(" Kazi Rafid Raiyan's At mega");
  delay(2000);
}

void loop() {
  //Serial.print("Receive Data : ");
  //Serial.println(Serial1.read());
  //delay(6000);
  StaticJsonBuffer<1000> jsonBuffer;
  int y = random(256,350);
  Serial.print("Send Data : ");
  Serial.println(y);
  JsonObject& S = jsonBuffer.createObject();
  S["status"] = y;
  S.printTo(Serial2);
  delay(500);
}
