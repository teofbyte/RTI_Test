#include <ModbusMaster.h>
#include <HardwareSerial.h>

#define MAX485_DE 4  // Pin DE/RE RS485
#define MAX485_RE 4

#define RXD1 16  // Power Meter RX
#define TXD1 17  // Power Meter TX
#define RXD2 26  // Temp Sensor RX
#define TXD2 25  // Temp Sensor TX

HardwareSerial powerSerial(1);
HardwareSerial tempSerial(2);

ModbusMaster powerMeter;
ModbusMaster tempSensor;

float temperature = 0.0;
float voltage = 0.0;
float current = 0.0;
float power = 0.0;

void preTransmission() {
  digitalWrite(MAX485_RE, 1);
}

void postTransmission() {
  digitalWrite(MAX485_RE, 0);
}

void setupsensor() {
  pinMode(MAX485_RE, OUTPUT);
  digitalWrite(MAX485_RE, 0);  // RS485 listen mode

  rs485Serial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);  // 9600 baud
  nodeTemp.begin(1, rs485Serial);                       // ID Modbus TX4S = 1
  nodePower.begin(2, rs485Serial);                      // ID Modbus EM4M = 2

  nodeTemp.preTransmission(preTransmission);
  nodeTemp.postTransmission(postTransmission);

  nodePower.preTransmission(preTransmission);
  nodePower.postTransmission(postTransmission);
}

float readTemperature() {
  uint8_t result = tempSensor.readInputRegisters(301001, 2);  // Ganti sesuai alamat register Autonics
  if (result == tempSensor.ku8MBSuccess) {
    uint16_t raw = tempSensor.getResponseBuffer(0);
    return raw / 10.0;  // misal data 245 → 24.5°C
  }
  return -1;

  // return random(250, 350) / 10.0;
}

float readVoltage() {
  uint8_t result = powerMeter.readInputRegisters(30006, 2);  // Ganti sesuai alamat Selec
  if (result == powerMeter.ku8MBSuccess) {
    return powerMeter.getResponseBuffer(0) / 10.0;
  }
  return -1

  // return random(250, 350) / 10.0;
}

float readCurrent() {
  uint8_t result = powerMeter.readInputRegisters(30022, 2);  // Ganti sesuai
  if (result == powerMeter.ku8MBSuccess) {
    return powerMeter.getResponseBuffer(0) / 100.0;
  }
  return -1;

  // return random(250, 350) / 10.0;
}

float readPower() {
  uint8_t result = powerMeter.readInputRegisters(30042, 2);  // Ganti sesuai
  if (result == powerMeter.ku8MBSuccess) {
    return powerMeter.getResponseBuffer(0) / 10.0;
  }
  return -1;

  // return random(250, 350) / 10.0;
}
