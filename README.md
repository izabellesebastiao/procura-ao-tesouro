# 🗺️ Caça ao Tesouro IoT com LoRa, GPS e Web em Tempo Real

Jogo de **caça ao tesouro** baseado em um sistema de rastreamento em tempo real.  
O “Tesouro” é um dispositivo móvel com GPS + LoRa, e o “Caçador” acompanha a posição em um **radar web** no smartphone, via nuvem.

> Do ponto de vista técnico, o projeto demonstra a integração prática entre **LoRa**, **ESP32**, **GPS**, **Firebase Realtime Database** e **GitHub Pages (HTTPS)** em uma arquitetura IoT.

---

## 🔍 Visão Geral

- **Objetivo**: criar um jogo de caça ao tesouro em que:
  - o **Tesouro** transmite sua localização via LoRa;
  - uma **Estação-Base** recebe esses dados e envia para a nuvem;
  - o **Caçador** acessa uma página web (HTTPS) no smartphone e vê:
    - distância até o Tesouro;
    - direção aproximada (radar).

- **Por que LoRa?**
  - Alcance de até alguns quilômetros;
  - Baixo consumo de energia;
  - Melhor alternativa que Wi-Fi/Bluetooth para dispositivos móveis de longo alcance.

---

## 🧩 Conceito do Jogo

- Jogo clássico de **Caça ao Tesouro**:
  - o Tesouro é escondido em algum lugar;
  - o Caçador precisa encontrá-lo.
- Nesta versão:
  - o Tesouro “sabe” sua própria localização (GPS);
  - envia a posição em tempo real via rádio LoRa;
  - o Caçador usa o **smartphone** para seguir as pistas no radar web.

---

## 🏗️ Arquitetura do Sistema

Principais componentes
  - Tesouro
    - Placa LILYGO TTGO T-Beam (ESP32 + LoRa + GPS NEO-6M/M8M)
    - Lê coordenadas GPS e envia via LoRa a cada X segundos.

  - Estação-Base (Gateway IoT)
    - Placa Heltec Wi-Fi LoRa V3 (ESP32 + LoRa SX1262 + OLED).
    - Recebe os pacotes LoRa do Tesouro.
    - Exibe coordenadas e RSSI no OLED.
    - Publica as coordenadas em tempo real no Firebase Realtime Database via Wi-Fi.
      
  - Radar Web (Front-end)
    - Página web hospedada em GitHub Pages (HTTPS).
        Obtém:
        - localização do Tesouro (Firebase);
        - localização do Caçador (API de Geolocalização do navegador).
        Calcula:
        - distância (fórmula de Haversine);
        - direção (bearing entre as coordenadas).
        - Exibe um radar em tempo real no smartphone.

---
          
## ⚙️ Tecnologias Utilizadas
  ## 🧩 Hardware
  
  **Tesouro (T-Beam)**  
      - LILYGO **TTGO T-Beam**.
      - ESP32.
      - LoRa **SX1276/SX1262**  
      - GPS **NEO-6M / M8M**
    
   **Estação-Base (Gateway IoT)**  
      - **Heltec Wi-Fi LoRa V3**  
      - ESP32  
      - LoRa **SX1262**  
      - Display **OLED integrado**
  
   **Caçador**  
      - Smartphone com:  
      - Navegador moderno (Chrome, Safari, etc.)  
      - GPS habilitado  

  ## 🧰 Software 

  - **IDE:** Arduino IDE  
  - **Linguagem:** C++

   **Tesouro (T-Beam)**  
      - `SPI.h` – comunicação de alta velocidade com chip LoRa  
      - `LoRa.h` – controle do rádio LoRa  
      - `TinyGPS++.h` – leitura e parse dos dados GPS (NMEA)
    
   **Estação-Base (Heltec)**  
      - `SPI.h` – comunicação com o módulo LoRa  
      - `RadioLib.h` – controle avançado do chip SX1262  
      - `Wire.h` – comunicação I²C com o display OLED  
      - `U8g2lib.h` – renderização de texto e gráficos no OLED  
      - `math.h` – cálculo Haversine e operações matemáticas  
    
   **Conectividade com a Nuvem**  
      - `WiFi.h` – conexão do ESP32 a redes Wi-Fi  
      - `HTTPClient.h` – envio de requisições HTTP ao Firebase  
      - `ArduinoJson.h` – montagem de objetos JSON com coordenadas e RSSI  

  ## ☁️ Nuvem e Front-end

   **Backend na Nuvem — Firebase Realtime Database**
      - Armazena sempre **a última posição** do Tesouro  
      - Sincronização **quase em tempo real**  
      - Acesso simples via API REST  
    
   **Front-end (Radar Web)**  
      **Hospedado em GitHub Pages (HTTPS obrigatório)**  
      - Tecnologias:  
        - HTML5  
        - CSS3  
        - JavaScript (Geolocation API + Firebase SDK)  
    
  **Funcionalidades:**  
      - Lê coordenadas do Tesouro (Firebase)  
      - Lê coordenadas do Caçador (GPS do smartphone, via HTTPS)  
      - Calcula distância (Haversine)  
      - Calcula direção (bearing em graus)  
      - Renderiza um **radar dinâmico** apontando para o Tesouro  

---

  ## 📡 Subsistema Local: LoRa + GPS

   **Fluxo Local (Offline)**  
      1. T-Beam lê coordenadas GPS via **UART** usando `TinyGPS++`  
      2. Formata em string:  
         `"-28.123456,-49.456789"`  
      3. Envia via **LoRa P2P**  
      4. Heltec V3 escuta continuamente (`RadioLib`)  
      5. Ao receber um pacote:  
         - decodifica coordenadas  
         - exibe latitude, longitude e **RSSI** no OLED  
    
   **Interfaces de Comunicação**
      - **UART** — GPS ↔ ESP32 (T-Beam)  
      - **SPI** — ESP32 ↔ LoRa (T-Beam e Heltec)  
      - **I²C** — ESP32 ↔ OLED (Heltec)  
      
  ## 🔧 Parâmetros LoRa
  Configuração otimizada para alcance + estabilidade:
      **Alcance prático:** 2–5 km em campo aberto  
      **Consumo:** baixo  
      **Confiabilidade:** alta  

  ## ☁️ Interface em Nuvem e Radar Web

   **Fluxo de Dados Fim-a-Fim**
    
   **1. Tesouro (T-Beam)**
      - Envia coordenadas via LoRa.
    
   **2. Estação-Base (Heltec)**
      - Recebe via LoRa.  
      - Monta JSON com:  
        - latitude  
        - longitude  
        - RSSI  
      - Envia via HTTP (Wi-Fi) para o Firebase.
      
   **3. Firebase**
      - Mantém sempre a **última posição** do Tesouro.  
      - Atualização rápida (ms).
    
   **4. Radar Web (GitHub Pages + JS)**
      - Lê coordenadas do Tesouro (Firebase).  
      - Lê coordenadas do Caçador (API de Geolocalização).  
      - Calcula:  
        - **distância** (Haversine)  
        - **direção** (bearing)  
      - Renderiza radar em tempo real.
    
   **Protocolos Utilizados**
      - **LoRa (915 MHz)** — Tesouro ↔ Estação-Base  
      - **Wi-Fi** — Estação-Base ↔ Internet  
      - **HTTP** — Heltec ↔ Firebase  
      - **HTTPS + Geolocation API** — Radar Web ↔ GPS do smartphone  

---

## ✅ Resultados Alcançados

### **Parte 1 – Subsistema Local (Offline)**
**T-Beam:**  
- Obtém coordenadas GPS corretamente  
- Envia via LoRa a cada ~10 s  

**Heltec:**  
- Recebe pacotes do Tesouro  
- Exibe coordenadas + RSSI no OLED  

**Conclusão:**  
- Comunicação LoRa P2P **estável**  
- Rastreamento **offline** funcionando perfeitamente  

  ## ⚠️ Riscos e Desafios Enfrentados

    ### **Instabilidade da biblioteca Heltec.h**
    - Problema: comportamento instável do chip  
    - Mitigação: uso de **RadioLib**, mais confiável
    
    ### **Incompatibilidade entre chips LoRa**
    - T-Beam: `LoRa.h` (SX1276/SX1262)  
    - Heltec: `RadioLib` (SX1262)  
    - Mitigação: alinhamento manual de SF, CR, BW e frequência
    
    ### **Restrição de GPS em HTTP**
    - Navegadores bloqueiam Geolocation API em HTTP  
    - Mitigação:  
      - GitHub Pages (HTTPS)  
      - Comunicação via nuvem em vez de conexão direta  
