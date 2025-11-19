# Projeto de Monitoramento de Quedas
**Sistema IoT com ESP32 + MPU6050 + Protocolo MQTT**

Este projeto implementa um sistema inteligente para detecção de quedas utilizando sensores inerciais, atuadores de alerta e conectividade com a nuvem (Adafruit IO). O sistema foi desenvolvido em C++ (Arduino) e pode ser testado tanto em hardware físico quanto por meio de simulação no Wokwi.

---

## 1. Descrição Resumida do Funcionamento

O projeto monitora constantemente os dados do acelerômetro e giroscópio do módulo MPU6050, conectados ao microcontrolador ESP32.
Com base nas leituras, o sistema:

1. Calcula a magnitude da aceleração e da velocidade angular.
2. Compara com limiares definidos (0.7 g para aceleração e 120°/s para giroscópio).
3. Caso a magnitude da aceleração fique abaixo de 0.7 g ou a magnitude do giroscópio ultrapasse 120°/s, o sistema detecta uma queda.
4. Quando a queda é detectada, o sistema:
   - Aciona o LED vermelho
   - Aciona o buzzer
   - Envia o valor **1** ao feed MQTT do Adafruit IO

Quando não há queda, o sistema:
- Mantém o LED verde aceso
- Buzzer desligado
- Envia o valor **0** para o feed MQTT do Adafruit IO

O envio MQTT ocorre a cada 2 segundos.

---

## 2. Como Reproduzir o Projeto

### **Configuração no Adafruit IO**
1. Acesse: `https://io.adafruit.com`.
2. Faça login na sua conta ou crie uma.
    Anote o seu Username, pois ele será utilizado para a conexão MQTT.
3. No menu, clique em `Feeds → New Feed`.
4. Preencha:
    - Name: (sua escolha)
    - Description: (opcional)
5. Clique em `Create`.
6. No feed criado, em `Settings → Feed Info`: Anote o seu `MQTT by Key` (SEU_USUARIO/feeds/FEED_KEY)

**Obtendo a chave AIO**
1. Clique na chave amarela no painel do Adafruit IO.
2. Anote a sua `Active Key`.

### **Hardware real**
1. Faça o upload do arquivo `Projeto_Monitoramento_de_Queda.ino`.
2. Edite diretamente no arquivo `.ino` os parâmetros:
   - Nome da rede Wi-Fi (`WIFI_SSID`)
   - Senha da rede (`WIFI_PASSWORD`)
   - Usuário MQTT do Adafruit IO
   - Chave AIO
   - Feed MQTT
3. Realize as conexões elétricas conforme tópico **5. Ligações Elétricas**.
4. Abra o painel no Adafruit IO e monitore o feed configurado.

### **Simulação no Wokwi**
1. Acesse o link de simulação: [Simulação no Wokwi](https://wokwi.com/projects/442893909925074945).
2. Importe o código `.ino`.
3. Substitua os parâmetros de configuração.
4. Execute a simulação e visualize o LED, o buzzer e o painel no Adafruit IO.

---

## 3. Software Desenvolvido (C++ / Arduino)

O arquivo principal do projeto é: `/Projeto_Monitoramento_de_Queda.ino`.

O software é responsável por:

- Inicializar o MPU6050 via I2C
- Medir aceleração e velocidade angular
- Calcular magnitudes dos vetores
- Detectar eventos de queda
- Controlar LEDs e buzzer
- Conectar ao Wi-Fi
- Publicar valores no MQTT/Adafruit IO

### Principais bibliotecas utilizadas

| Biblioteca | Função |
|-----------|--------|
| Wire.h | Comunicação I2C |
| Adafruit_MPU6050.h e Adafruit_Sensor.h | Leitura de sensores do módulo MPU6050 |
| PubSubClient.h | Comunicação MQTT |
| WiFi.h | Conexão com a rede Wi-Fi |

### Parâmetros importantes do algoritmo

- Limiar de aceleração: **0.7 g**
- Limiar de giroscópio: **120°/s**
- Tempo entre verificações: **2 segundos**
- LEDs: verde para normalidade / vermelho para queda
- Buzzer: desativado para normalidade / ativado para queda

---

## 4. Hardware Utilizado

### Microcontrolador
- **ESP32 DevKit V1**
  - Tensão: 3.3V
  - Wi-Fi 802.11 b/g/n
  - Bluetooth 4.2

### Sensor inercial
- **MPU6050**
  - Acelerômetro 3 eixos
  - Giroscópio 3 eixos
  - Interface I2C (endereço padrão 0x68)

### Atuadores
- LED verde
- LED vermelho
- Buzzer (passivo)

---

## 5. Ligações Elétricas (Pinout)

### **Conexão ESP32 → MPU6050 (I2C)**

| MPU6050 | ESP32 |
|---------|--------|
| VCC | 3.3V |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |

### **LEDs e Buzzer**

| Componente | ESP32 | Observações |
|-----------|-------|-------------|
| LED Verde | GPIO 25 | Com resistor de 220 Ω |
| LED Vermelho | GPIO 26 | Com resistor de 220 Ω |
| Buzzer | GPIO 27 | Buzzer passivo, GND no GND |

---

## 6. Interfaces, Protocolos e Comunicação

### **MQTT – Adafruit IO**
- Protocolo: MQTT
- Broker: `io.adafruit.com`
- Autenticação: usuário + chave AIO
- Feed configurado diretamente no código: `SEU_USUARIO/feeds/FEED_KEY`
- Mensagens publicadas: **1** → queda detectada / **0** → normalidade

### Publicação MQTT a cada 2s:
```text
queda: 0 ou 1
```

---

## 7. Equipe / Autoria

### Projeto desenvolvido como parte da disciplina **Objetos Inteligentes Conectados**
- Grupo: 13 (Andrea, Carlos e Lucas)
- Universidade Presbiteriana Mackenzie
- Data: 2025-11-19
