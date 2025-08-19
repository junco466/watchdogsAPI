#include <SPI.h>
#include <LoRa.h>

// Pines LoRa SX1278 (RA-02)
#define LORA_SCK     18
#define LORA_MISO    19
#define LORA_MOSI    23
#define LORA_SS      5
#define LORA_RST     14
#define LORA_DIO0    2

// Frecuencia LoRa (debes usar la misma en ambos módulos)
#define LORA_BAND   433E6 //915E6(depende de la capacidad el modulo) Cambia a 868E6 o 433E6 si estás en Europa o Asia y tu módulo es compatible

void setup() {
  Serial.begin(115200);
  while (!Serial);

  Serial.println("Iniciando LoRa Transmisor...");

  // Configura pines SPI LoRa
  SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_SS);
  
  // Reset manual del módulo LoRa
  pinMode(LORA_RST, OUTPUT);
  digitalWrite(LORA_RST, LOW);
  delay(20);
  digitalWrite(LORA_RST, HIGH);
  delay(20);

  // Configura LoRa con pines definidos
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(LORA_BAND)) {
    Serial.println("Error al inicializar LoRa!");
    while (1);
  }

  Serial.println("LoRa inicializado correctamente.");
}

void loop() {
  Serial.println("Enviando mensaje...");

  LoRa.beginPacket();
  LoRa.print("Hola ");
  LoRa.endPacket();

  delay(3000); // Espera 3 segundos para volver a enviar
}
