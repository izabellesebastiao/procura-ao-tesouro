//programa heltec -> estação base do caça ao tesouro

#include <SPI.h>
#include <RadioLib.h> //biblioteca q controla o Lora
#include <U8g2lib.h>  //controla display
#include <Wire.h> //comunicação i2c
#include <math.h>     

// Coordenadas fixa do heltec, estacao base.
const double ESTACAO_BASE_LAT = -28.953000; 
const double ESTACAO_BASE_LON = -49.475000;


// Definições de Pinos
#define VEXT_CTRL 21 //pino de energia
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

// criar os Objetos
SX1262 radio = new Module(LORA_CS, LORA_DIO1, LORA_RST, LORA_BUSY); //cria o radio
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, OLED_RST, OLED_SCL, OLED_SDA); //cria o display

// Variáveis globais
String g_lat = "N/A";
String g_lon = "N/A";
String g_rssi = "N/A";
String g_dist = "---"; 

// Cálculo de Haversine
double deg2rad(double deg) {
  return (deg * M_PI / 180); // converte graus para radianos
}

// Calcula a distância em metros entre dois pontos GPS
double haversineDistance(double lat1, double lon1, double lat2, double lon2) {
  double R = 6371000; // Raio da Terra em metros
  double dLat = deg2rad(lat2 - lat1);
  double dLon = deg2rad(lon2 - lon1);
  double a = sin(dLat / 2) * sin(dLat / 2) +
             cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
             sin(dLon / 2) * sin(dLon / 2);
  double c = 2 * atan2(sqrt(a), sqrt(1 - a));
  return R * c; // Distância em metros
}

//inicializações
void setup() {
  Serial.begin(115200); //inicializa a porta serial
  Serial.println("FASE 1.1: Heltec (Calculando Distancia)");
  Serial.print("Estacao-Base localizada em: ");
  Serial.print(ESTACAO_BASE_LAT, 6); Serial.print(", "); Serial.println(ESTACAO_BASE_LON, 6);

  pinMode(VEXT_CTRL, OUTPUT);
  digitalWrite(VEXT_CTRL, HIGH); //liga a energia
  delay(100);

  Wire.begin(OLED_SDA, OLED_SCL); //inicializa comunicação I2C
  display.begin(); //inicializa o display, limpa memoria
  
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  int state = radio.begin(LORA_FREQUENCY);
  if (state != RADIOLIB_ERR_NONE) {
    Serial.print("Falha ao iniciar RadioLib: "); Serial.println(state); while (1);
  }

  display.clearBuffer();
  display.setFont(u8g2_font_helvR08_tr);
  display.drawStr(0, 15, "Display OK.");
  display.drawStr(0, 30, "LoRa OK.");
  display.drawStr(0, 45, "Aguardando GPS...");
  display.sendBuffer(); //manda o conteudo para a tela
}

void loop() {
  String receivedText = "";
 
  int state = radio.receive(receivedText); //ouve o sinal de radio

  if (state == RADIOLIB_ERR_NONE) {
    g_rssi = String(radio.getRSSI()) + " dBm";
    int commaIndex = receivedText.indexOf(',');

    if (commaIndex > 0) {
      // Coordenada válida recebida
      g_lat = receivedText.substring(0, commaIndex);
      g_lon = receivedText.substring(commaIndex + 1);
      
      // --- CÁLCULO ---
      double tesouro_lat = g_lat.toDouble();
      double tesouro_lon = g_lon.toDouble();

    } else {
      g_lat = receivedText;
      g_lon = "...";
    }

    // Atualiza o display
    display.clearBuffer();
    display.setFont(u8g2_font_helvR10_tr); 
    display.drawStr(0, 14, ("transmitindo..."));
    display.setFont(u8g2_font_helvB08_tr); 
    display.drawStr(0, 30, ("Lat: " + g_lat).c_str());
    display.drawStr(0, 44, ("Lon: " + g_lon).c_str());
    display.setFont(u8g2_font_helvR08_tr); 
    display.drawStr(0, 60, ("Sinal: " + g_rssi).c_str());
    display.sendBuffer();
  }
}