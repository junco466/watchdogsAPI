#include <SPI.h>
#include <LoRa.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager

String inString = "";

// Pines para ESP32 (cambiar según conexión)
#ifdef ESP32
#define LORA_SCK  18
#define LORA_MISO 19
#define LORA_MOSI 23
#define LORA_SS   5
#define LORA_RST  14
#define LORA_DIO0 27
#endif

void setup() {
  Serial.begin(115200);
  Serial.println("Prendio el ESP32");
  //while (!Serial);

  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);
  Serial.println("Es ESP32");

  if (!LoRa.begin(433E6)) { // Cambiar a 915E6 o 868E6 según módulo/región
    Serial.println("Falla LoRa *******");
    while (1);
  }
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    inString = "";
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      inString += c;
    }

    Serial.print("Rx: ");
    Serial.println(inString);

    Serial.print("Potencia Señal: ");
    Serial.println(LoRa.packetRssi()); // -40dBm muy buena, -70 buena, -100 débil
  }
  delay(50);
}
