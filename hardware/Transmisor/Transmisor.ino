#include <SPI.h>
#include <LoRa.h>
#include <ArduinoJson.h>

#define SS      5
#define RST     14
#define DIO0    2

void setup() {
  Serial.begin(9600);
  while (!Serial);

  LoRa.setPins(SS, RST, DIO0);

  Serial.println("Iniciando transmisor LoRa...");
  if (!LoRa.begin(915E6)) {
    Serial.println("Fallo al iniciar LoRa");
    while (1);
  }
  Serial.println("LoRa listo");
}

void loop() {
  float lat = 6.25184;
  float lon = -75.56359;

  // Crear objeto JSON
  StaticJsonDocument<128> doc;
  doc["lat"] = lat;
  doc["lon"] = lon;

  String jsonStr;
  serializeJson(doc, jsonStr);

  // Enviar por LoRa
  LoRa.beginPacket();
  LoRa.print(jsonStr);
  LoRa.endPacket();

  Serial.println("Mensaje enviado: " + jsonStr);
  delay(5000);
}
