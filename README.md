# objetos-inteligentes-conectados
C++
// Inclusão das Bibliotecas Essenciais
#include <WiFi.h>             // Para conectividade Wi-Fi
#include <Adafruit_MPU6050.h>   // Para o sensor MPU6050
#include <Adafruit_Sensor.h>    // Biblioteca base para sensores Adafruit
#include <Wire.h>               // Para comunicação I2C
#include "Adafruit_MQTT.h"       // Biblioteca base para MQTT
#include "Adafruit_MQTT_Client.h"// Cliente MQTT para Adafruit IO

// --- Configurações de Rede Wi-Fi ---
// Substitua com os dados da sua rede
#define WLAN_SSID       "NOME_DA_SUA_REDE_WIFI"
#define WLAN_PASS       "SENHA_DA_SUA_REDE_WIFI"

// --- Configurações da Plataforma Adafruit IO ---
// Substitua com suas credenciais da Adafruit IO
#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883
#define AIO_USERNAME    "SEU_USUARIO_ADAFRUIT"
#define AIO_KEY         "SUA_CHAVE_ADAFRUIT_IO"

// --- Definição dos Pinos dos Atuadores ---
const int PINO_LED_VERDE = 25;      // LED de status normal
const int PINO_LED_VERMELHO = 26;   // LED de alerta de queda
const int PINO_BUZZER = 27;         // Alarme sonoro

// --- Limiares de Deteção de Queda ---
const float LIMIAR_ACELERACAO = 0.7; // em g
const float LIMIAR_GIROSCOPIO = 120.0; // em graus/s

// --- Instância dos Objetos e Variáveis Globais ---
Adafruit_MPU6050 mpu;
WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
Adafruit_MQTT_Publish feedQueda = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/queda");

bool quedaDetectada = false; // Flag para controlar o estado de queda

// --- Função de Conexão Wi-Fi ---
void conectaWiFi() {
  Serial.print("Conectando ao Wi-Fi...");
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status()!= WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" Conectado!");
}

// --- Função de Conexão MQTT ---
void conectaMQTT() {
  int8_t ret;
  if (mqtt.connected()) {
    return; // Se já estiver conectado, não faz nada
  }
  Serial.print("Conectando ao Broker MQTT...");
  uint8_t retries = 3;
  while ((ret = mqtt.connect())!= 0) { // Tenta conectar
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Tentando novamente em 5 segundos...");
    mqtt.disconnect();
    delay(5000);
    retries--;
    if (retries == 0) {
      while (1); // Bloqueia se não conseguir conectar após 3 tentativas
    }
  }
  Serial.println(" Conectado ao MQTT!");
}

// --- Configuração Inicial (Executa uma vez) ---
void setup() {
  Serial.begin(115200);

  // Inicializa os pinos dos atuadores como saída
  pinMode(PINO_LED_VERDE, OUTPUT);
  pinMode(PINO_LED_VERMELHO, OUTPUT);
  pinMode(PINO_BUZZER, OUTPUT);

  // Inicializa o sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("Falha ao encontrar o MPU6050. Verifique a conexão.");
    while (1);
  }
  Serial.println("MPU6050 encontrado!");

  // Conecta ao Wi-Fi
  conectaWiFi();
}

// --- Loop Principal (Executa continuamente) ---
void loop() {
  // Garante que a conexão MQTT está ativa a cada ciclo
  conectaMQTT();

  // Mantém a conexão MQTT viva e processa pacotes
  mqtt.processPackets(10000);
  if(!mqtt.ping()) {
    mqtt.disconnect();
  }

  // Lê os dados do sensor
  sensors_event_t a, g, temp;
  mpu.getEvent(&a, &g, &temp);

  // Calcula a magnitude dos vetores de aceleração e giroscópio
  float moduloA = sqrt(pow(a.acceleration.x, 2) + pow(a.acceleration.y, 2) + pow(a.acceleration.z, 2));
  float moduloG = sqrt(pow(g.gyro.x, 2) + pow(g.gyro.y, 2) + pow(g.gyro.z, 2));

  // Lógica de deteção de queda baseada nos limiares
  if (moduloA > LIMIAR_ACELERACAO |

| moduloG > LIMIAR_GIROSCOPIO) {
    quedaDetectada = true;
  } else {
    quedaDetectada = false;
  }

  // Atua com base na deteção
  if (quedaDetectada) {
    // Estado de Queda: aciona alertas locais e envia "1" via MQTT
    digitalWrite(PINO_LED_VERDE, LOW);
    digitalWrite(PINO_LED_VERMELHO, HIGH);
    tone(PINO_BUZZER, 4000);
    Serial.println("Queda Detectada! Enviando '1' para o Adafruit IO.");
    feedQueda.publish(1);
  } else {
    // Estado Normal: desliga alertas e envia "0" via MQTT
    digitalWrite(PINO_LED_VERDE, HIGH);
    digitalWrite(PINO_LED_VERMELHO, LOW);
    noTone(PINO_BUZZER);
    Serial.println("Estado Normal. Enviando '0' para o Adafruit IO.");
    feedQueda.publish(0);
  }

  // Aguarda 2 segundos antes da próxima leitura
  delay(2000);
}
