#include <WiFi.h>
#include "ThingSpeak.h"

// Cambia esto por tus datos
const char* ssid = "";
const char* password = "";
const char* thingSpeakApiKey = "";
unsigned long channelNumber = ;

WiFiClient client;

int value = 0;
int direction = 1;  // 1 = sube, -1 = baja

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi conectado!");
  ThingSpeak.begin(client);
}

void loop() {
  // Enviar valor actual al canal
  int statusCode = ThingSpeak.writeField(channelNumber, 1, value, thingSpeakApiKey);

  if (statusCode == 200) {
    Serial.print("Enviado a ThingSpeak: ");
    Serial.println(value);
  } else {
    Serial.print("Error al enviar: ");
    Serial.println(statusCode);
  }

  // Cambiar el valor para la siguiente iteración
  value += direction;

  // Cambiar dirección si alcanzamos los extremos
  if (value >= 20) {
    direction = -1;
  } else if (value <= 0) {
    direction = 1;
  }

  delay(20000);  // Esperar 20 segundos por límites de ThingSpeak (mínimo 15 s)
}
