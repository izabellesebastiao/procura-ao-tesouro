#include <WiFi.h>       // Para ligar ao Wi-Fi de casa
#include <HTTPClient.h> // Para fazer o upload para a nuvem
#include <ArduinoJson.h> // Para criar o JSON
#include <SPI.h>        // Para o LoRa
#include <RadioLib.h>   // Para o LoRa
#include <U8g2lib.h>    // Para o Display
#include <Wire.h>       // Para o Display

// --- informaçoes de login do wifi

const char* SEU_WIFI_SSID = "iPhone de izabelle Custodia";
const char* SEU_WIFI_PASS = "99116622";

// --- URL do Firebase. 
String FIREBASE_URL = "https://radar-tesouro-esp32-default-rtdb.firebaseio.com/.json";


// --- Pinos (Heltec V3) ---
#define VEXT_CTRL 21
#define OLED_SDA 17
#define OLED_SCL 18
#define OLED_RST 21
#define LORA_SCK  9
#define LORA_MISO 11
#define LORA_MOSI 10
#define LORA_CS   8
#define LORA_RST  12
#define LORA_DIO1 14
#define LORA_BUSY 13
#define LORA_FREQUENCY 915.0 

// --- Objetos ---
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY);
U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, OLED_RST, OLED_SCL, OLED_SDA);
WiFiClient client;
HTTPClient http;

volatile bool packetFlag = false;

ICACHE_RAM_ATTR void onReceive(void) {
  packetFlag = true;
}

// Função para ligar ao Wi-Fi 
void setupWiFi() {
  WiFi.begin(SEU_WIFI_SSID, SEU_WIFI_PASS);
  Serial.print("Ligando ao Wi-Fi: ");
  Serial.println(SEU_WIFI_SSID);
  u8g2.drawStr(0, 30, "Ligando ao Wi-Fi..."); //print no diplay do heltec
  u8g2.sendBuffer();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Ligado!");
}

void setup() {
  Serial.begin(115200);
  Serial.println("--- Heltec (Uploader para a Nuvem) ---");

  // 1. Inicializa Display e Energia
  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, HIGH);
  delay(100);
  Wire.begin(OLED_SDA, OLED_SCL);
  u8g2.begin();
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0, 15, "Heltec Uploader");
  u8g2.sendBuffer();

  // 2. Liga ao Wi-Fi
  setupWiFi();
  
  // 3. Inicializa LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  int state = radio.begin(LORA_FREQUENCY);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Falha LoRa: "); Serial.println(state); while (1);
  }
  radio.setDio1Action(onReceive);
  radio.startReceive();
  
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_helvR08_tr);
  u8g2.drawStr(0, 15, "Wi-Fi OK!");
  u8g2.drawStr(0, 30, "LoRa OK.");
  u8g2.drawStr(0, 45, "Aguardando GPS...");
  u8g2.sendBuffer();
}

void loop() {
  if (packetFlag) {
    packetFlag = false;
    String receivedText = "";
    int state = radio.readData(receivedText);

    if (state == RADIOLIB_ERR_NONE) {
      String rssi = String(radio.getRSSI()) + " dBm";
      
      int commaIndex = receivedText.indexOf(',');
      if (commaIndex > 0) {
        String lat = receivedText.substring(0, commaIndex);
        String lon = receivedText.substring(commaIndex + 1);
        
        // Atualiza o display OLED
        u8g2.clearBuffer();
        u8g2.setFont(u8g2_font_helvR08_tr);
        u8g2.drawStr(0, 12, "GPS Recebido!");
        u8g2.drawStr(0, 30, ("Lat: " + lat).c_str());
        u8g2.drawStr(0, 44, ("Lon: " + lon).c_str());
        u8g2.drawStr(0, 60, "A fazer upload...");
        u8g2.sendBuffer();

        // --- FAZ O UPLOAD PARA A NUVEM ---
        // Cria o documento JSON
        StaticJsonDocument<200> doc;
        doc["lat"] = lat.toDouble();
        doc["lon"] = lon.toDouble();
        doc["rssi"] = rssi;
        doc["status"] = "OK";

        String jsonPayload;
        serializeJson(doc, jsonPayload);
        
        // Envia para o Firebase
        http.begin(FIREBASE_URL);
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.PUT(jsonPayload); // PUT substitui os dados
        
        if (httpResponseCode > 0) {
          Serial.print("Upload OK! Resposta: ");
          Serial.println(httpResponseCode);
          u8g2.drawStr(0, 60, "Upload OK!");
          u8g2.sendBuffer();
        } else {
          Serial.print("Upload FALHOU! Erro: ");
          Serial.println(httpResponseCode);
          u8g2.drawStr(0, 60, "Upload FALHOU!");
          u8g2.sendBuffer();
        }
        http.end();
        // --------------------------------
      }
    }
    radio.startReceive();
  }
}