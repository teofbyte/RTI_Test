#include <WiFi.h>
#include <MQTT.h>
#include <ArduinoJson.h>
#include <Sensor.h>

WiFiClient net;
MQTTClient client;

const char *ssid = "Matthew_IoT";
const char *pass = "tutorialsetup";
const char *broker = "broker.emqx.io";
const char *clientid = "node1";
const char *topic = "DATA/LOCAL/SENSOR/PANEL_1";

int qos = 1;
bool retain = true;
String status = "OK";
String deviceID = "Teofilus";
String Fan = "OFF";
int pinFan = 27;

// Nilai sensor global
float temp = 0, volt = 0, amp = 0, pa = 0;

// Handle task
TaskHandle_t TaskMQTT;
TaskHandle_t TaskSensor;

void setup() {
  Serial.begin(115200);
  pinMode(pinFan, OUTPUT);
  setupsensor();

  WiFi.begin(ssid, pass);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println("\nWiFi sudah terhubung");

  client.begin(broker, net);

  // Membuat task untuk MQTT dan sensor
  xTaskCreatePinnedToCore(mqttTask, "MQTTTask", 4096, NULL, 1, &TaskMQTT, 0);
  xTaskCreatePinnedToCore(sensorTask, "SensorTask", 4096, NULL, 1, &TaskSensor, 1);
}

void mqttTask(void *parameter) {
  for (;;) {
    if (!client.connected()) {
      connectMQTT();
    }
    client.loop();
    vTaskDelay(10 / portTICK_PERIOD_MS); // Loop MQTT setiap 10 ms
  }
}

void sensorTask(void *parameter) {
  for (;;) {
    // Baca sensor
    temp = readTemperature();
    volt = readVoltage();
    amp = readCurrent();
    pa = readPower();

    // Kontrol fan berdasarkan suhu
    if(temp >= 27.54){
      Fan = "ON";
      digitalWrite(pinFan, 1);
    }else{
      Fan = "OFF";
      digitalWrite(pinFan, 0);
    }

    // Buat dan kirim JSON
    String json = buatJSONStatus(status, deviceID, String(volt), String(amp), String(pa), String(temp), Fan);
    client.publish(topic, json, retain, qos);

    // Delay antar publish (1 detik)
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
}

void connectMQTT() {
  Serial.print("Menghubungkan ke Broker MQTT");
  while (!client.connect(clientid)) {
    Serial.print(".");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }
  Serial.println("\nSudah Terhubung ke Broker");
}

String buatJSONStatus(String status, String deviceID, String v, String i, String pa, String temp, String fan) {
  StaticJsonDocument<256> doc;

  doc["status"] = status;
  doc["deviceID"] = deviceID;

  JsonObject data = doc.createNestedObject("data");
  data["v"] = v;
  data["i"] = i;
  data["pa"] = pa;
  data["temp"] = temp;
  data["fan"] = fan;

  String jsonOutput;
  serializeJson(doc, jsonOutput);
  return jsonOutput;
}

void loop() {
  
}
