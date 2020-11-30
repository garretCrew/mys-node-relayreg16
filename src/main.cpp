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
#define RELAY_REGISTER_16 0
#define TEMP_SENSOR 1
#define REPORT_TIME 60000

#define DS_PIN 3
#define STCP_PIN 5
#define SHCP_PIN 4
//#define OE_PIN 6
#define ONE_WIRE_BUS 7
#define MAX_ATTACHED_DS18B20 1

#include <MySensors.h>
#include <MsTimer2.h>
#include <DallasTemperature.h>
#include <OneWire.h>

unsigned int relRegLastStat = 0;

MyMessage msgRelReg16(RELAY_REGISTER_16,V_CUSTOM);
MyMessage msgTempSens(TEMP_SENSOR, V_TEMP);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

void reportStatus() {
  send(msgRelReg16.set(relRegLastStat, 0));
  
  sensors.requestTemperatures();
  int16_t conversionTime = sensors.millisToWaitForConversion(sensors.getResolution());
  sleep(conversionTime);
  float temperature = static_cast<float>(static_cast<int>((getControllerConfig().isMetric?sensors.getTempCByIndex(0):sensors.getTempFByIndex(0)) * 10.)) / 10.;

  //if (lastTemperature != temperature && temperature != -127.00 && temperature != 85.00) {
  if (temperature != -127.00 && temperature != 85.00) {
    send(msgTempSens.set(temperature,0));
  }
}

void preHwInit() {
  pinMode(DS_PIN, OUTPUT);  
  pinMode(STCP_PIN, OUTPUT);
  pinMode(SHCP_PIN, OUTPUT);
  digitalWrite(STCP_PIN, LOW);
  shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, 255);
  shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, 255);   
  digitalWrite(STCP_PIN, HIGH);
}

void before() {
  sensors.begin();
}

void presentation() { 
  sendSketchInfo("Rel array[16] & temp sens", "0.2");
  present(RELAY_REGISTER_16, S_CUSTOM);
  present(TEMP_SENSOR, S_TEMP);
}

void setup() {
  sensors.setWaitForConversion(false);
  MsTimer2::set(REPORT_TIME, reportStatus);
  MsTimer2::start();
}

void loop() {}

void receive(const MyMessage &message) {
  if (message.getSensor()==RELAY_REGISTER_16 && message.getType()==V_CUSTOM) {
    digitalWrite(STCP_PIN, LOW);
    delay(10);
    shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, ~message.getUInt()/256);
    shiftOut(DS_PIN, SHCP_PIN, MSBFIRST, ~message.getUInt()%256);
    delay(10);
    digitalWrite(STCP_PIN, HIGH);
    relRegLastStat = message.getUInt();
    send(msgRelReg16.set(relRegLastStat, 0));
  }
}