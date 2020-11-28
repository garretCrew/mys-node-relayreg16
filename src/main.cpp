//#define MY_DEBUG

#define MY_RADIO_RFM69
//#define MY_RFM69_NEW_DRIVER
#define MY_RFM69_FREQUENCY RFM69_433MHZ
#define MY_RFM69_TX_POWER_DBM 10
//#define MY_RFM69_ENABLE_ENCRYPTION
//#define MY_ENCRYPTION_SIMPLE_PASSWD "xxxxx"
#define MY_RFM69_NETWORKID 100
#define MY_PARENT_NODE_ID 254

#define MY_NODE_ID 10
#define RELAY_REGISTER_16 1
#define TEMP_SENSOR 2

#define DS_PIN 3
#define STCP_PIN 5
#define SHCP_PIN 4
#define OE_PIN 6

#include <MySensors.h>  

MyMessage msg(RELAY_REGISTER_16,V_CUSTOM);

void preHwInit() {
  pinMode(DS_PIN, OUTPUT);  
  pinMode(STCP_PIN, OUTPUT);
  pinMode(SHCP_PIN, OUTPUT);
  digitalWrite(STCP_PIN, LOW);
  shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, 255);
  shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, 255);   
  digitalWrite(STCP_PIN, HIGH);
}

void presentation() { 
  sendSketchInfo("Rel array[16] & temp sens", "0.2");
  present(RELAY_REGISTER_16, S_CUSTOM);
  present(TEMP_SENSOR, S_TEMP);
}

void setup() {}

void loop() {}

void receive(const MyMessage &message) {
  if (message.getSensor()==RELAY_REGISTER_16 && message.getType()==V_CUSTOM) {
    digitalWrite(STCP_PIN, LOW);
    delay(10);
    shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, ~message.getUInt()/256);
    shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, ~message.getUInt()%256);
    delay(10);
    digitalWrite(STCP_PIN, HIGH);
    send(msg.set(message.getUInt(), 0));
  }
}
