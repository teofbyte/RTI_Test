#include <ModbusMaster.h>
#include <HardwareSerial.h>

#define MAX485_DE 4  // Pin DE/RE RS485
#define MAX485_RE 4

#define TX_PIN 17  // TX ke RS485
#define RX_PIN 16  // RX dari RS485

ModbusMaster nodeTemp;   // Untuk TX4S
ModbusMaster nodePower;  // Untuk Selec EM4M

HardwareSerial rs485Serial(2);  // Gunakan UART2 (GPIO16 RX, GPIO17 TX)

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
  uint8_t result = nodeTemp.readInputRegisters(301001, 1);  // Address & length tergantung TX4S
  if (result == nodeTemp.ku8MBSuccess) {
    uint16_t raw = nodeTemp.getResponseBuffer(0);
    return raw / 10.0;  // Misal data suhu 1 desimal, sesuaikan dengan datasheet TX4S
  }
  return -999.0;

  // return random(250, 350) / 10.0;
}

float readVoltage() {
  uint8_t result = nodePower.readInputRegisters(30006, 2);  // Cek alamat di datasheet EM4M
  if (result == nodePower.ku8MBSuccess) {
    uint32_t raw = ((uint32_t)nodePower.getResponseBuffer(0) << 16) | nodePower.getResponseBuffer(1);
    return raw / 100.0;  // Contoh konversi, tergantung datasheet
  }
  return -1;
  // return random(250, 350) / 10.0;
}

float readCurrent() {
  uint8_t result = nodePower.readInputRegisters(30022, 2);
  if (result == nodePower.ku8MBSuccess) {
    uint32_t raw = ((uint32_t)nodePower.getResponseBuffer(0) << 16) | nodePower.getResponseBuffer(1);
    return raw / 1000.0;  // Contoh: 1 digit = mA
  }
  return -1;
  // return random(250, 350) / 10.0;
}

float readPower() {
  uint8_t result = nodePower.readInputRegisters(30042, 2);
  if (result == nodePower.ku8MBSuccess) {
    uint32_t raw = ((uint32_t)nodePower.getResponseBuffer(0) << 16) | nodePower.getResponseBuffer(1);
    return raw / 100.0;
  }
  return -1;
  // return random(250, 350) / 10.0;
}
