#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>

// Configurações do LoRa 
#define LORA_FREQUENCY 915E6
#define LORA_SCK    5
#define LORA_MISO   19
#define LORA_MOSI   27
#define LORA_CS     18
#define LORA_RST    23
#define LORA_DIO0   26
// Configuraçoes radio lora
#define LORA_SPREADING_FACTOR   9 //alcance 
#define LORA_CODING_RATE        7 //correção de erros
#define LORA_BANDWIDTH          125E3 //largura da banda

// Configurações do GPS
TinyGPSPlus gps; // variavel que dara a localiz
HardwareSerial ss(Serial2); //def qual porta serial vai ser usada, serial2 
#define ESP_RX_PIN_PARA_GPS 12
#define ESP_TX_PIN_PARA_GPS 15

unsigned long lastSendTime = 0; // Controla o envio
int counter = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println(" FASE 1: impplementação do T-Beam ");

  // Inicializa o GPS 
  ss.begin(9600, SERIAL_8N1, ESP_RX_PIN_PARA_GPS, ESP_TX_PIN_PARA_GPS);
  Serial.println("GPS iniciado com sucesso!");

  //  Inicializa o LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);
  LoRa.setPins(LORA_CS, LORA_RST, LORA_DIO0);
  if (!LoRa.begin(LORA_FREQUENCY)) {
    Serial.println("Erro ao iniciar o LoRa! Travando.");
    while (1);
  }
  LoRa.setSpreadingFactor(LORA_SPREADING_FACTOR);
  LoRa.setSignalBandwidth(LORA_BANDWIDTH);
  LoRa.setCodingRate4(LORA_CODING_RATE);
  Serial.println("LoRa iniciado com sucesso!");
}

// Função para ler o GPS
void readGPS() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
  }
}

void loop() {
  // O GPS deve ser lido constantemente
  readGPS();

  // Vamos enviar a localização a cada 10 segundos
  if (millis() - lastSendTime > 10000) {
    lastSendTime = millis();

    // Verifica se temos uma localização VÁLIDA
    if (gps.location.isValid()) {
      // Cria a mensagem
      String lat = String(gps.location.lat(), 6);
      String lng = String(gps.location.lng(), 6);
      String message = lat + "," + lng;

      // Envia o pacote LoRa
      LoRa.beginPacket();
      LoRa.print(message);
      LoRa.endPacket();

      Serial.print("Pacote #");
      Serial.print(counter++);
      Serial.print(" enviado: ");
      Serial.println(message);

    } else {
      Serial.println("Procurando Satelites...");
      LoRa.beginPacket();
      LoRa.print("Procurando...");
      LoRa.endPacket();
    }
  }
}