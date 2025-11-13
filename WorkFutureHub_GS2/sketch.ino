/* WorkFuture Hub - GS2
Autores: Geovana Maria da Silva Cardoso - 566254
         Mariana Silva do Egito Moreira - 562544
Turma: 1ESPF

Descrição: Sistema de monitoramento ambiental inteligente que coleta dados de sensores,
calcula eficiência energética e envia para FIWARE via MQTT.

Ligações:
DHT22         - D15
LDR           - D34
PIR           - D4 (5V)
Potenciômetro - D35
LED Verde     - D5
LED Vermelho  - D2
Buzzer        - D18
LCD VCC       - 5V
LCD SDA       - D21
LCD SCL       - D22
*/

#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <LiquidCrystal_I2C.h>

// ===== CONFIGURAÇÕES WIFI/MQTT =====
const char* SSID = "Wokwi-GUEST";
const char* PASSWORD = "";
const char* BROKER_MQTT = "20.220.27.106";
const int BROKER_PORT = 1883;
const char* ID_MQTT = "workfuture_hub_001";

// ===== TÓPICOS MQTT ATUALIZADOS =====
const char* TOPICO_PUBLISH_DATA = "/TEF/workfuture001/attrs";
const char* TOPICO_SUBSCRIBE_CMD = "/TEF/workfuture001/cmd";

// ===== CONFIGURAÇÃO DOS SENSORES =====
#define DHTPIN 15
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// ===== PINOS =====
#define LDR_PIN 34
#define PIR_PIN 4
#define POT_PIN 35
#define LED_VERDE 5
#define LED_VERMELHO 2
#define BUZZER_PIN 18

// ===== LCD I2C =====
LiquidCrystal_I2C lcd(0x27, 16, 2); // Endereço 0x27, 16 colunas x 2 linhas

// ===== VARIÁVEIS GLOBAIS =====
WiFiClient espClient;
PubSubClient MQTT(espClient);

// Variáveis dos sensores
float temperatura = 0;
float umidade = 0;
int luminosidade = 0;
bool presenca = false;
int nivelRuido = 0;
float eficienciaEnergetica = 0;

// Estados do sistema
bool sistemaAtivo = true;

// ===== TIMING CONTROL =====
unsigned long lastSensorRead = 0;
unsigned long lastEfficiencyCalc = 0;
unsigned long lastDataSend = 0;
unsigned long lastLCDUpdate = 0;

const unsigned long INTERVALO_SENSORES = 2000;    // 2 segundos
const unsigned long INTERVALO_EFICIENCIA = 5000;  // 5 segundos
const unsigned long INTERVALO_ENVIO = 10000;      // 10 segundos
const unsigned long INTERVALO_LCD = 3000;         // 3 segundos

// ===== PROTÓTIPOS DE FUNÇÃO =====
void initSerial();
void initWiFi();
void initMQTT();
void initSensores();
void initLCD();
void reconectWiFi();
void reconnectMQTT();
void mqtt_callback(char* topic, byte* payload, unsigned int length);
void verificaConexoes();
void lerSensores();
void calcularEficiencia();
void exibirNoLCD();
void controlarAlertas();
void enviarDadosMQTT();
String criarPayloadJSON();

// ===== SETUP =====
void setup() {
  initSerial();
  initSensores();
  initWiFi();
  initMQTT();
  initLCD();
  
  Serial.println("✅ WorkFuture Hub Inicializado");
  lcd.clear();
  lcd.print("WorkFuture Hub");
  lcd.setCursor(0, 1);
  lcd.print("  Inicializado  ");
  delay(2000);
}

// ===== LOOP PRINCIPAL =====
void loop() {
  verificaConexoes();
  
  unsigned long currentMillis = millis();
  
  // Leitura de sensores a cada 2 segundos
  if (currentMillis - lastSensorRead >= INTERVALO_SENSORES) {
    lerSensores();
    lastSensorRead = currentMillis;
  }
  
  // Cálculo de eficiência a cada 5 segundos
  if (currentMillis - lastEfficiencyCalc >= INTERVALO_EFICIENCIA) {
    calcularEficiencia();
    controlarAlertas(); // Controlar alertas junto com eficiência
    lastEfficiencyCalc = currentMillis;
  }
  
  // Atualização do LCD a cada 3 segundos
  if (currentMillis - lastLCDUpdate >= INTERVALO_LCD) {
    exibirNoLCD();
    lastLCDUpdate = currentMillis;
  }
  
  // Envio para MQTT a cada 10 segundos
  if (currentMillis - lastDataSend >= INTERVALO_ENVIO) {
    enviarDadosMQTT();
    lastDataSend = currentMillis;
  }
  
  MQTT.loop();
  delay(100); // Pequena pausa para estabilidade
}

// ===== INICIALIZAÇÕES =====
void initSerial() {
  Serial.begin(115200);
  Serial.println();
  Serial.println("🚀 WorkFuture Hub - Iniciando...");
}

void initWiFi() {
  delay(10);
  Serial.println("📡 Conectando WiFi...");
  Serial.print("SSID: ");
  Serial.println(SSID);
  
  WiFi.begin(SSID, PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println("\n✅ WiFi Conectado!");
  Serial.print("📱 IP: ");
  Serial.println(WiFi.localIP());
}

void initMQTT() {
  MQTT.setServer(BROKER_MQTT, BROKER_PORT);
  MQTT.setCallback(mqtt_callback);
}

void initSensores() {
  // Configurar pinos
  pinMode(LED_VERDE, OUTPUT);
  pinMode(LED_VERMELHO, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(PIR_PIN, INPUT);
  
  // Inicializar sensores
  dht.begin();
  
  // LEDs iniciais apagados
  digitalWrite(LED_VERDE, LOW);
  digitalWrite(LED_VERMELHO, LOW);
  noTone(BUZZER_PIN);
  
  Serial.println("✅ Sensores inicializados");
}

void initLCD() {
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.print("WorkFuture Hub");
  Serial.println("✅ LCD inicializado");
}

// ===== SISTEMA DE SENSORES =====
void lerSensores() {
  // Ler DHT22 (Temperatura e Umidade)
  float temp = dht.readTemperature();
  float umid = dht.readHumidity();
  
  if (!isnan(temp)) temperatura = temp;
  if (!isnan(umid)) umidade = umid;
  
  // Ler LDR (Luminosidade) - 0-4095 para 0-100%
  int ldrValue = analogRead(LDR_PIN);
  luminosidade = map(ldrValue, 0, 4095, 0, 100);
  
  // Ler PIR (Presença)
  presenca = digitalRead(PIR_PIN);
  
  // Ler Potenciômetro (Nível de Ruído simulado) - 0-4095
  nivelRuido = analogRead(POT_PIN);
  
  // Debug no Serial - APENAS QUANDO MUDAR VALORES SIGNIFICATIVAMENTE
  static float lastTemp = 0;
  static float lastUmid = 0;
  static int lastLuz = 0;
  static bool lastPresenca = false;
  static int lastRuido = 0;
  
  bool changed = (abs(temperatura - lastTemp) > 0.5) || 
                 (abs(umidade - lastUmid) > 1) || 
                 (abs(luminosidade - lastLuz) > 5) || 
                 (presenca != lastPresenca) || 
                 (abs(nivelRuido - lastRuido) > 100);
  
  if (changed) {
    Serial.println("=== DADOS DOS SENSORES ===");
    Serial.printf("🌡️  Temp: %.1f°C | 💧 Umidade: %.1f%%\n", temperatura, umidade);
    Serial.printf("💡 Luz: %d%% | 👤 Presença: %s\n", luminosidade, presenca ? "SIM" : "NÃO");
    Serial.printf("🔊 Ruído: %d | ⚡ Eficiência: %.1f%%\n", nivelRuido, eficienciaEnergetica * 100);
    Serial.println("==========================");
    
    lastTemp = temperatura;
    lastUmid = umidade;
    lastLuz = luminosidade;
    lastPresenca = presenca;
    lastRuido = nivelRuido;
  }
}

void calcularEficiencia() {
  float pontuacao = 100.0; // Começa com 100%
  
  // Fator 1: Luminosidade + Presença (Desperdício energético)
  if (luminosidade > 70 && !presenca) {
    pontuacao -= 30; // Grande penalidade por luz acesa sem ninguém
    Serial.println("⚠️  ALERTA: Desperdício energético - Luz acesa sem presença");
  } else if (luminosidade > 70 && presenca) {
    pontuacao += 5; // Bônus por uso eficiente
  }
  
  // Fator 2: Temperatura ideal (18-26°C)
  if (temperatura < 18 || temperatura > 26) {
    pontuacao -= 20; // Penalidade por temperatura fora da faixa
    Serial.printf("⚠️  ALERTA: Temperatura fora da faixa ideal (%.1f°C)\n", temperatura);
  } else {
    pontuacao += 10; // Bônus por temperatura ideal
  }
  
  // Fator 3: Umidade ideal (40-70%)
  if (umidade < 40 || umidade > 70) {
    pontuacao -= 15;
    Serial.printf("⚠️  ALERTA: Umidade fora da faixa ideal (%.1f%%)\n", umidade);
  } else {
    pontuacao += 5;
  }
  
  // Fator 4: Ruído elevado (acima de 2500)
  if (nivelRuido > 2500) {
    pontuacao -= 10;
    Serial.printf("⚠️  ALERTA: Nível de ruído elevado (%d)\n", nivelRuido);
  }
  
  // Limitar entre 0% e 100%
  eficienciaEnergetica = constrain(pontuacao / 100.0, 0.0, 1.0);
  
  Serial.printf("📊 Eficiência calculada: %.1f%%\n", eficienciaEnergetica * 100);
}

// ===== INTERFACE LCD =====
void exibirNoLCD() {
  static int displayMode = 0;
  lcd.clear();

  switch (displayMode) {
    // ===== Tela 1: Temperatura e Umidade =====
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Temp:");
      lcd.print(temperatura, 1);
      lcd.print((char)223); // símbolo de grau
      lcd.print("C");

      lcd.setCursor(0, 1);
      lcd.print("Umid:");
      lcd.print(umidade, 0);
      lcd.print("%   "); // espaçamento
      break;

    // ===== Tela 2: Luminosidade e Eficiência =====
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Luz:");
      lcd.print(luminosidade);
      lcd.print("%   ");

      lcd.setCursor(0, 1);
      lcd.print("Eff:");
      lcd.print(eficienciaEnergetica * 100, 0);
      lcd.print("%   ");
      break;

    // ===== Tela 3: Presença e Ruído =====
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Pres:");
      lcd.print(presenca ? "SIM " : "NAO");

      lcd.setCursor(0, 1);
      lcd.print("Ruido:");
      lcd.print(nivelRuido);
      lcd.print("   ");
      break;
  }

  displayMode = (displayMode + 1) % 3; // alterna entre 3 telas
}


// ===== SISTEMA DE ALERTAS =====
void controlarAlertas() {
  bool condicoesIdeais = true;
  
  // Verificar condições críticas
  if (temperatura < 16 || temperatura > 30) {
    condicoesIdeais = false;
  }
  
  if (luminosidade > 70 && !presenca) {
    condicoesIdeais = false;
  }
  
  if (nivelRuido > 3000) {
    condicoesIdeais = false;
  }
  
  if (eficienciaEnergetica < 0.5) {
    condicoesIdeais = false;
  }
  
  // Controlar LEDs e Buzzer
  if (condicoesIdeais) {
    digitalWrite(LED_VERDE, HIGH);
    digitalWrite(LED_VERMELHO, LOW);
    noTone(BUZZER_PIN);
  } else {
    digitalWrite(LED_VERDE, LOW);
    digitalWrite(LED_VERMELHO, HIGH);
    
    // Buzzer intermitente para alertas (apenas bip rápido)
    tone(BUZZER_PIN, 1000, 200);
  }
}

// ===== COMUNICAÇÃO MQTT =====
void mqtt_callback(char* topic, byte* payload, unsigned int length) {
  String mensagem;
  
  for (int i = 0; i < length; i++) {
    mensagem += (char)payload[i];
  }
  
  Serial.print("📨 Mensagem MQTT Recebida: ");
  Serial.print(mensagem);
  Serial.print(" - Topico ");
  Serial.println(topic);
  
  // Comandos para controle remoto do sistema
  if (String(topic) == TOPICO_SUBSCRIBE_CMD) {
    if (mensagem == "system_on") {
      sistemaAtivo = true;
      Serial.println("✅ Sistema ativado remotamente");
    } else if (mensagem == "system_off") {
      sistemaAtivo = false;
      Serial.println("⏸️  Sistema pausado remotamente");
    }
  }
}

void verificaConexoes() {
  if (!MQTT.connected()) {
    reconnectMQTT();
  }
  
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("🔌 WiFi desconectado, reconectando...");
    reconectWiFi();
  }
}

void reconectWiFi() {
  if (WiFi.status() == WL_CONNECTED) return;
  
  WiFi.begin(SSID, PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n✅ WiFi Reconectado!");
}

void reconnectMQTT() {
  while (!MQTT.connected()) {
    Serial.print("🔌 Conectando ao Broker MQTT...");
    
    if (MQTT.connect(ID_MQTT)) {
      Serial.println("✅ Conectado!");
      
      // Inscrever nos tópicos de comando
      MQTT.subscribe(TOPICO_SUBSCRIBE_CMD);
      Serial.println("✅ Inscrito nos tópicos de comando");
      
    } else {
      Serial.print("❌ Falha. Código: ");
      Serial.print(MQTT.state());
      Serial.println(" Tentando novamente em 5s...");
      delay(5000);
    }
  }
}

void enviarDadosMQTT() {
  if (!MQTT.connected()) {
    Serial.println("❌ MQTT não conectado");
    return;
  }
  
  String payload = criarPayloadJSON();
  
  Serial.print("📤 Publicando no tópico: ");
  Serial.println(TOPICO_PUBLISH_DATA);
  Serial.print("📦 Payload: ");
  Serial.println(payload);
  
  if (MQTT.publish(TOPICO_PUBLISH_DATA, payload.c_str())) {
    Serial.println("✅ Dados enviados para FIWARE via MQTT");
  } else {
    Serial.println("❌ Falha ao publicar no MQTT");
  }
}

String criarPayloadJSON() {
  // FORMATO ULTRALIGHT COM OBJECT_IDS (igual ao provisionamento)
  String payload = "temperature|" + String(temperatura, 1) + 
                   "|humidity|" + String(umidade, 1) + 
                   "|light|" + String(luminosidade) + 
                   "|presence|" + String(presenca ? "true" : "false") + 
                   "|noise_level|" + String(nivelRuido) + 
                   "|energy_efficiency|" + String(eficienciaEnergetica, 2);
  
  return payload;
}