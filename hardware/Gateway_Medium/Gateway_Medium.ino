#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SS      5
#define RST     14
#define DIO0    2

const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";
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

    // Validar JSON recibido
    StaticJsonDocument<128> doc;
    DeserializationError error = deserializeJson(doc, mensaje);

    if (error) {
      Serial.print("Error al parsear JSON: ");
      Serial.println(error.c_str());
      return;  // no enviamos nada si el mensaje está mal
    }

    // Opcional: acceder a valores y verificar que existan
    if (!doc.containsKey("lat") || !doc.containsKey("lon")) {
      Serial.println("JSON no contiene lat o lon");
      return;
    }

    float lat = doc["lat"];
    float lon = doc["lon"];

    Serial.print("Latitud: "); Serial.println(lat, 6);
    Serial.print("Longitud: "); Serial.println(lon, 6);

    // Enviar al backend
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      String jsonOut;
      serializeJson(doc, jsonOut);

      int httpResponseCode = http.POST(jsonOut);
      Serial.print("HTTP code: ");
      Serial.println(httpResponseCode);
      Serial.println("Respuesta: " + http.getString());

      http.end();
    }
  }
}
