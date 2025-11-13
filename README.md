# WorkFuture Hub

O **WorkFuture Hub** é um sistema inteligente de monitoramento ambiental IoT projetado para **avaliar e otimizar as condições de trabalho em ambientes corporativos**, medindo temperatura, umidade, luminosidade, presença e ruído, e calculando um **índice de eficiência energética**. O sistema gera alertas locais e remotos quando parâmetros saem da faixa ideal, promovendo **saúde, bem-estar e eficiência organizacional**.

---
### 📋 Autoras

- Geovana Maria da Silva Cardoso - 566254  
- Mariana Silva do Egito Moreira - 562544  
- 1ESPF - Engenharia de Software

---
### 🚜️ Componentes e Sensores

- **ESP32 DevKit** – microcontrolador principal
- **DHT22** – sensor de temperatura e umidade
- **LDR** – sensor de luminosidade
- **PIR** – sensor de presença
- **Potenciômetro** – simula nível de ruído
- **Buzzer** e **LEDs** – alertas sonoros e visuais
- **LCD I2C** – exibe leituras e ícones personalizados de cada sensor

![Montagem do Projeto](dispWokGS.png)

---
### 🌐 Arquitetura IoT com FIWARE

O sistema integra **ESP32** com FIWARE, utilizando **IoT Agent MQTT**, **Orion Context Broker** e **STH-Comet** para armazenamento histórico. A arquitetura permite **monitoramento em tempo real**, análise de séries temporais e controle remoto do ESP32.

### Fluxo de Dados

1. **ESP32**: coleta dados dos sensores e aplica regras de alerta
2. **MQTT Broker (Mosquitto)**: recebe mensagens do ESP32
3. **IoT Agent MQTT**: traduz dados MQTT para entidades NGSI
4. **Orion Context Broker**: armazena o contexto atual de cada ambiente
5. **MongoDB**: persistência de entidades
6. **STH-Comet**: histórico de registros para análises temporais
7. **Backend Node.js/Express**: processa dados, aplica regras, disponibiliza APIs
8. **Frontend Web**: dashboard responsivo com gráficos em tempo real e envio de comandos MQTT

![Arquitetura do Projeto](arqWorkFuture.png)

---
### ⚙️ Funcionalidades do Dispositivo e Sistema

- Monitoramento de **temperatura, umidade, luminosidade, presença e ruído** em tempo real
- Cálculo do **índice de eficiência energética** com base nas leituras
- Alertas locais via **buzzer e LEDs** quando parâmetros saem da faixa segura
- Publicação contínua de dados no **broker MQTT**, compatível com FIWARE
- Recepção de **comandos remotos** via dashboard para ativar ou desativar o sistema
- Exibição de leituras no **LCD I2C** com ícones personalizados
- Dashboard web interativo com gráficos e indicadores de eficiência em tempo real

---
## 🎥 Apresentação do Projeto

[Vídeo Explicação](#)  

## 🎥 Demonstração do Sistema em Operação Simulada

[Vídeo Demonstração](#)

---
# 📊 Dashboard Dinâmico

### Tela Principal
![Tela Principal](telainiGS.png)

### Gráfico de Temperatura e Umidade Luminosidade
![Gráfico Sensor](graficoTempUmi.png)

### Gráfico de Luminosidade e Presença 
![Gráfico Luminosidade](graficoLumi.png)

### Gráfico de Ruído
![Gráfico Ruído](graficoRuido.png)

### Gráfico do Índice de Eficiência Energética
![Gráfico Eficiência](graficoEficiencia.png)

---
## Funcionamento do Sistema

### Frontend (Dashboard Web)
- Exibe dados de **temperatura, umidade, luminosidade, presença, ruído e eficiência energética**
- Atualiza automaticamente em tempo real
- Destaca visualmente leituras fora da faixa ideal
- Permite enviar comandos para o ESP32, como **ativar/desativar alertas** ou **desligar o sistema**
- Visualização clara de histórico de dados

### Backend (API Express + MQTT + FIWARE)
- Recebe dados do **ESP32 via MQTT**
- Consulta **STH-Comet** para histórico
- Calcula **índice de eficiência energética** e aplica regras de alerta
- Disponibiliza **endpoints para frontend**
- Permite envio de **comandos MQTT para controle remoto do ESP32**

### Regras de Alertas
- **Temperatura** fora de 20–25°C → LED/Buzzer ativo
- **Umidade** fora de 40–60% → alerta
- **Luminosidade** alta sem **Presença** → alerta
- **Ruído acima do limite** → alerta

### Fluxo de Dados
1. ESP32 coleta sensores e aplica regras
2. Dados enviados via MQTT para FIWARE
3. Backend processa dados e disponibiliza APIs
4. Frontend atualiza dashboards em tempo real
5. Comandos do dashboard são enviados via MQTT para o ESP32

---
### Fluxo de Dados Resumido
ESP32 → MQTT → Backend → Frontend → Comandos MQTT → ESP32

---
### 🔧 Configurações Editáveis

- **Wi-Fi**: SSID e PASSWORD
- **MQTT**: IP do broker, porta, tópicos e ID do dispositivo
- **Sensores**: pinos de DHT22, LDR, PIR e potenciômetro
- **Alertas**: parâmetros de limite para LED/Buzzer
- **Intervalo de leitura**: tempo entre leituras dos sensores (padrão 2s)

---
## 🧪 Testes e Validação

### Testes do Dispositivo (Wokwi)
- Simulação do ESP32, sensores, buzzer e LCD
- Verificação de logs no Serial Monitor

### Testes da Integração (Postman + FIWARE)
- Criação de entidades no Orion Context Broker
- Consulta de dados em tempo real
- Verificação de histórico via STH-Comet
- Simulação de envio de comandos MQTT
- [Vídeo Teste com Postman](#)


---
### ✅ Benefícios do Sistema

- Monitoramento ambiental em tempo real  
- Cálculo de **índice de eficiência energética** para análise de produtividade  
- Alertas locais e remotos garantindo **saúde e bem-estar no ambiente de trabalho**  
- Persistência de dados históricos para análises futuras  
- Escalabilidade para múltiplos ambientes simultaneamente  

---
## 📁 Estrutura do Código

- **Configurações**: rede Wi-Fi, MQTT, sensores e parâmetros de alerta  
- **Inicializações**: serial, Wi-Fi, MQTT e sensores  
- **Loop principal**: leitura de sensores, cálculo do índice de eficiência, verificação de alertas, envio MQTT  
- **Funções auxiliares**: conexão Wi-Fi/MQTT, leitura de sensores, cálculo de índice de eficiência, ativação/desativação de alertas, publicação MQTT  

---
## 📎 Links Importantes
- 🔗 [Projeto no Wokwi](#)  
- 🎥 [Vídeo Explicativo](#)  
- 🎥 [Vídeo Demonstração](#)  
- 📂 [Collection Postman](#)

---
## 📖 Instruções de Uso (Simulação no Wokwi)

### 1️⃣ Acessando a Simulação
1. Abra o link do projeto  
2. Clique em **Start Simulation**  
3. Visualize leituras e alertas no **Serial Monitor**

### 2️⃣ Interagindo com os Sensores
- **DHT22** → temperatura e umidade  
- **LDR** → luminosidade  
- **PIR** → presença  
- **Potenciômetro** → simula ruído  
- **Buzzer e LEDs** → alertas visuais e sonoros  

### 3️⃣ Acompanhando os Dados
- Serial Monitor mostra: temperatura, umidade, luminosidade, presença, ruído, índice de eficiência  
- Alertas acionados automaticamente quando fora da faixa segura

### 4️⃣ Testando Situações
- **Ambiente quente**: ajuste DHT22 > 25°C  
- **Baixa luminosidade**: reduza LDR  
- **Ruído alto**: aumente potenciômetro  
- Observe alertas e atualização do índice de eficiência  

---
### 5️⃣ Integração com FIWARE

1. Configure IP do **broker MQTT** no código  
2. Inicie componentes FIWARE: Mosquitto, IoT Agent MQTT, Orion e STH-Comet  
3. Importe a **Collection Postman** para criar entidades e acompanhar dados do ESP32  

---
### 6️⃣ Encerrando a Simulação
- Clique em **Stop Simulation**  
- Dados e logs serão reiniciados ao iniciar novamente  

---
### 📌 Referências

Código e Arquitetura orientados pelo professor Fábio Henrique Cabrini

**Autoras:** Geovana Maria da Silva Cardoso e Mariana Silva do Egito Moreira

