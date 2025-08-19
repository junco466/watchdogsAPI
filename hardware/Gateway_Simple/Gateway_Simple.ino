#include <SPI.h>
#include <LoRa.h>
#include <WiFi.h>
#include <HTTPClient.h>

// Pines LoRa
#define SS      5
#define RST     14
#define DIO0    2

// Wi-Fi
const char* ssid = "TU_SSID";
const char* password = "TU_PASSWORD";

// URL del backend Flask
const char* serverUrl = "http://TU_IP_LOCAL:5000/api/ubicacion";

void setup() {
  Serial.begin(9600);

  // Iniciar Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a WiFi...");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi conectado");

  // Iniciar LoRa
  LoRa.setPins(SS, RST, DIO0);
  if (!LoRa.begin(915E6)) {  // Cambiar a 433E6 o 868E6 según tu módulo
    Serial.println("Fallo al inicializar LoRa.");
    while (1);
  }
  Serial.println("Receptor LoRa listo");
}

void loop() {
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    String mensaje = "";
    while (LoRa.available()) {
      mensaje += (char)LoRa.read();
    }

    Serial.println("Mensaje recibido vía LoRa: " + mensaje);

    // Aquí asumimos que el mensaje contiene datos tipo JSON ya válidos
    if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;
      http.begin(serverUrl);
      http.addHeader("Content-Type", "application/json");

      int httpResponseCode = http.POST(mensaje);
      Serial.print("Código de respuesta HTTP: ");
      Serial.println(httpResponseCode);

      String response = http.getString();
      Serial.println("Respuesta del servidor: " + response);
      http.end();
    } else {
      Serial.println("Error: WiFi desconectado");
    }
  }
}
