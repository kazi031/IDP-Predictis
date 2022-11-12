#include <HardwareSerial.h>
#include <ArduinoJson.h>

HardwareSerial MySerial(1);

void setup() {
    Serial.begin(115200);
    MySerial.begin(115200, SERIAL_8N1, 16, 17);
}

void loop() {
    //int x = random(0,255);
    //Serial.print("Send Data : ");
    //Serial.println(x);
    StaticJsonBuffer<1000> jsonBuffer;
    //MySerial.write(x);
    JsonObject& S = jsonBuffer.parseObject(Serial);
    //delay(6000);
    int Status = S["status"];
    Serial.print("Receive Data : ");
    Serial.println(Status);
    delay(500);
}
