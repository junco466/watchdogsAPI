#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SS      5
#define RST     14
#define DIO0    2

const char* ssid = "TU_SSID";
const char* password = "TU_PASS";
const char* serverUrl = "http://TU_IP_LOCAL:5000/api/ubicacion";

void setup() {
  Serial.begin(9600);

  // Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  // LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(915E6)) {
    Serial.println("Fallo al iniciar LoRa.");
    while (1);
  }
  Serial.println("LoRa receptor listo");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String mensaje = "";
    while (LoRa.available()) {
      mensaje += (char)LoRa.read();
    }

    Serial.println("Mensaje recibido: " + mensaje);

    StaticJsonDocument<256> doc;
    DeserializationError error = deserializeJson(doc, mensaje);

    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      return;
    }

    // Validar existencia de campos
    if (!doc.containsKey("device_id") || !doc.containsKey("token") || !doc.containsKey("lat") || !doc.containsKey("lon")) {
      Serial.println("Faltan campos obligatorios");
      return;
    }

    // Validar device_id
    String deviceID = doc["device_id"];
    if (deviceID != "TX001" && deviceID != "TX002") {
      Serial.println("Dispositivo no autorizado");
      return;
    }

    // Validar token
    String token = doc["token"];
    if (token != "abc123") {
      Serial.println("Token inválido");
      return;
    }

    // Datos válidos, enviar al backend
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonOut;
      serializeJson(doc, jsonOut);

      int httpCode = http.POST(jsonOut);
      Serial.print("HTTP Code: ");
      Serial.println(httpCode);
      Serial.println("Respuesta: " + http.getString());

      http.end();
    }
  }
}
