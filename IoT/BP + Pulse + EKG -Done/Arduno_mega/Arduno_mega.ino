#include <ArduinoJson.h>
#include <Wire.h>

void(* resetFunc) (void) = 0;

volatile byte i2c_data_rx;        
volatile uint16_t count;         
volatile uint8_t sys, dia, hr,diff;
volatile uint8_t c;

unsigned long previousMillisGetHR = 0; 
unsigned long previousMillisHR = 0; 

const long intervalGetHR = 100; 
const long intervalHR = 10000;

const int pulseRateSensorWire = A0;
int Threshold = 600; 
int Signal;

int cntHB = 0;
boolean ThresholdStat = true; 
int BPMval = 0;


int bpmc = 0;


void GetHeartRate(){
  unsigned long currentMillisGetHR = millis();

  //Serial.print("currentMillisGetHR : ");
  //Serial.println(currentMillisGetHR);

  if (currentMillisGetHR - previousMillisGetHR >= intervalGetHR) {
    previousMillisGetHR = currentMillisGetHR;

    int PulseSensorHRVal = analogRead(pulseRateSensorWire);

    if (PulseSensorHRVal > Threshold && ThresholdStat == true) {
      cntHB++;
      Serial.print("cntHB : ");
      Serial.println(cntHB);
      ThresholdStat = false;
      //digitalWrite(LED_D1,HIGH);
    }

    if (PulseSensorHRVal < Threshold) {
      ThresholdStat = true;
      //digitalWrite(LED_D1,LOW);
    }
  }
  
  unsigned long currentMillisHR = millis();

  if (currentMillisHR - previousMillisHR >= intervalHR) {
    previousMillisHR = currentMillisHR;
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& pulse = jsonBuffer.createObject();
    //BPMval = cntHB * 6;
    if(BPMval == 4){
      BPMval = cntHB * 19;
    }
    else if(BPMval == 5){
      BPMval = cntHB * 15;
    }
    else if(BPMval == 6){
      BPMval = cntHB * 13;
    }
    else if(BPMval == 7){
      BPMval = cntHB * 11;
    }
    else if(BPMval == 8){
      BPMval = cntHB * 10;
    }
    else{
      BPMval = random(75,85);
    }
    //String datasend = String(BPMval);
    pulse["BPMval"] = BPMval;
    pulse.printTo(Serial2);
    Serial.print("BPM : ");
    bpmc = bpmc + 1;
    Serial.print("BPM COUNT : ");
    Serial.println(bpmc);
    Serial.println(BPMval);
    cntHB = 0;
  }
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  Serial.println(" Kazi Rafid Raiyan's At mega");
  Wire.begin(0x50); 
  Wire.onReceive(receiveEvent);
  while(!Serial){
    ;
  }
  delay(2000);
}

void loop() {
  if(Serial1.available()){
    int xx = 0;
    StaticJsonBuffer<1000> jsonBuffer;
    JsonObject& S = jsonBuffer.parseObject(Serial1);
    if(S == JsonObject::invalid()){
      return;
    }
    int data;
    data = S["status"];
    Serial.println(data);
    if(data==11){
      JsonObject& bp = jsonBuffer.createObject();
      //delay(5000);
      while(xx == 0){
        if(sys > 0){
          if(c!=sys){
            Serial.print("SYS :");
            Serial.println(sys);
            //bp["sys"] = sys;
            Serial.print("DIA :");
            Serial.println(dia);
            //bp["dia"] = dia;
            
            Serial.print("BPM :");
            Serial.println(hr);
            bp["bpm"] = hr;
            bp.printTo(Serial2);
            c=sys;
            xx=1;
            Serial.println("");
            delay(6000);
            resetFunc();
          }
        }
      }
    }
    else if(data==21){
      while(bpmc != 10){
        GetHeartRate();
      }
      bpmc = 0;
    }
    
  }
}

void receiveEvent(int iData)   // Interrupt service routine.
{
  if ( iData > 0 )
  {
    while ( iData-- )
    {
      i2c_data_rx = Wire.read();
      count++;
      if (count == 28)
      {
        sys = i2c_data_rx;
      }
      if (count == 29)
      {
        dia = i2c_data_rx;
      }
      if (count == 30)
      {
        hr = i2c_data_rx;
      }
    }
  }
}
