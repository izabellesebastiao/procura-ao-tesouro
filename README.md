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

### Visão de alto nível

```mermaid
flowchart LR
    T[Tesouro<br/>T-Beam (ESP32 + LoRa + GPS)]
      -->|Coordenadas via LoRa| H[Estação-Base<br/>Heltec ESP32 LoRa V3]

    H -->|HTTP + JSON via Wi-Fi| F[Firebase Realtime Database]

    F -->|HTTPS + API JS| W[Radar Web<br/>GitHub Pages + Smartphone]

    W -->|Geolocalização (GPS)| C[Caçador]
