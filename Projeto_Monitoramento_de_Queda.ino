/*
Projeto: Monitoramento de Queda
Autor: Equipe 13 (Andrea, Carlos e Lucas)
Descrição: Detecta padrões de queda via MPU6050 e publica no Adafruit IO (MQTT).
Arquivo: Projeto_Monitoramento_de_Queda.ino
Data: 2025-11-19
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""
#define WIFI_CHANNEL 6

const char* mqttServer = "io.adafruit.com";
const int mqttPort = 1883;
const char* mqttUserName = "INSERIR MQTT USERNAME";
const char* mqttPassword = "INSERIR MQTT PASSWORD";
const char* mqttFeed = "INSERIR MQTT FEED";

WiFiClient espClient;
PubSubClient mqttClient(espClient);
Adafruit_MPU6050 mpu;

#define LED_VERDE  25
#define LED_VERMELHO 26
#define BUZZER 27

int quedaDetectada = 0;
const float LIMIAR_ACEL = 0.7;
const float LIMIAR_GIRO = 120.0;

void setup() {
  Serial.begin(115200);
  
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  digitalWrite(LED_VERDE, HIGH);

  while (!mpu.begin()) {
    Serial.println("MPU6050 não conectado!");
    delay(1000);
  }
  Serial.println("MPU6050 pronto!");

  conectaWiFi();
  mqttClient.setServer(mqttServer, mqttPort);
}

void loop() {
  if (!mqttClient.connected()) conectaMQTT();
  mqttClient.loop();

  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  float ax = a.acceleration.x / 9.80665;
  float ay = a.acceleration.y / 9.80665;
  float az = a.acceleration.z / 9.80665;
  float gx = g.gyro.x * 57.2958;
  float gy = g.gyro.y * 57.2958;
  float gz = g.gyro.z * 57.2958;

  float moduloA = sqrt(ax*ax + ay*ay + az*az);
  float moduloG = sqrt(gx*gx + gy*gy + gz*gz);

  if (moduloA < LIMIAR_ACEL || moduloG > LIMIAR_GIRO) {
    quedaDetectada = 1;
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    tone(BUZZER, 4000);
  } else {
    quedaDetectada = 0;
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    noTone(BUZZER);
  }

  // Publica no feed
  char payload[2];
  snprintf(payload, sizeof(payload), "%d", quedaDetectada);
  if(mqttClient.publish(mqttFeed, payload)) {
    Serial.print("Enviado: ");
    Serial.println(payload);
  } else {
    Serial.println("Falha no envio!");
  }

  delay(2000);
}

void conectaWiFi() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD, WIFI_CHANNEL);
  Serial.print("Conectando ao WiFi ");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(200);
  }
  Serial.println(" Conectado!");
  Serial.print("IP: "); Serial.println(WiFi.localIP());
}

void conectaMQTT() {
  while (!mqttClient.connected()) {
    Serial.print("Conectando ao Adafruit MQTT...");
    String clientID = "ESP32Client-" + String(random(0xffff), HEX);
    if (mqttClient.connect(clientID.c_str(), mqttUserName, mqttPassword)) {
      Serial.println(" conectado!");
    } else {
      Serial.print(" falha, rc=");
      Serial.println(mqttClient.state());
      delay(3000);
    }
  }
}
