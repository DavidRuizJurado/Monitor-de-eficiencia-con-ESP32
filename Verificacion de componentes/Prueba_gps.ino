#include <Arduino.h>
#include <TinyGPS++.h>
#include <HardwareSerial.h>
#include <time.h> // Necesaria para los cálculos de fecha

// Objeto para decodificar los datos del GPS
TinyGPSPlus gps;

// Usaremos el puerto 'Serial2' del ESP32.
HardwareSerial GPS_Serial(2);

// Pines para el GPS en Serial2 por defecto
const int GPS_RX_PIN = 16;
const int GPS_TX_PIN = 17;

// Prototipo de la función que calcula el offset
int getSpainUTCOffset();

void setup() {
  // Inicia la comunicación serie con el ordenador
  Serial.begin(115200);
  
  // Inicia la comunicación con el módulo GPS
  GPS_Serial.begin(9600, SERIAL_8N1, GPS_RX_PIN, GPS_TX_PIN);

  Serial.println("Inicializado. Buscando señal GPS con cambio de hora automático...");
}

void loop() {
  // Leemos y procesamos los datos del módulo GPS
  while (GPS_Serial.available() > 0) {
    if (gps.encode(GPS_Serial.read())) {
      displayInfo(); // Si se completa una trama, mostramos la información
      delay(1000);
    }
  }
}

// Función para mostrar los datos del GPS en el Monitor Serie
void displayInfo() {
  if (!gps.location.isValid() || !gps.date.isValid() || !gps.time.isValid()) {
    // No mostramos nada si no tenemos los datos básicos
    return;
  }

  // --- OBTENER OFFSET AUTOMÁTICAMENTE ---
  int utcOffset = getSpainUTCOffset();

  // --- UBICACIÓN ---
  Serial.print("Ubicación: ");
  Serial.print(gps.location.lat(), 6);
  Serial.print(", ");
  Serial.print(gps.location.lng(), 6);

  // --- VELOCIDAD ---
  Serial.print(" | Velocidad: ");
  Serial.print(gps.speed.kmph());
  Serial.print(" km/h");

  // --- FECHA Y HORA (AJUSTADA AUTOMÁTICAMENTE) ---
  Serial.print(" | Fecha/Hora (España): ");
  Serial.print(gps.date.day());
  Serial.print("/");
  Serial.print(gps.date.month());
  Serial.print("/");
  Serial.print(gps.date.year());
  
  int localHour = (gps.time.hour() + utcOffset) % 24;
  Serial.print(" ");
  if (localHour < 10) Serial.print("0");
  Serial.print(localHour);
  Serial.print(":");
  if (gps.time.minute() < 10) Serial.print("0");
  Serial.print(gps.time.minute());
  Serial.print(":");
  if (gps.time.second() < 10) Serial.print("0");
  Serial.print(gps.time.second());
  Serial.print(utcOffset == 2 ? " (CEST)" : " (CET)");


  // --- SATÉLITES ---
  Serial.print(" | Satélites: ");
  Serial.print(gps.satellites.value());
  
  Serial.println(); // Salto de línea
}


/**
 * @brief Calcula el desfase UTC para España (Península) teniendo en cuenta el DST.
 * @return +2 para horario de verano (CEST), +1 para horario de invierno (CET).
 */
int getSpainUTCOffset() {
  if (!gps.date.isValid() || !gps.time.isValid()) {
    return 1; // Devuelve el valor por defecto si no hay fecha
  }

  int year = gps.date.year();
  int month = gps.date.month();
  int day = gps.date.day();
  int hour = gps.time.hour(); // Hora UTC

  // Reglas generales:
  // De Enero a Febrero -> Invierno (UTC+1)
  // De Abril a Septiembre -> Verano (UTC+2)
  // Noviembre y Diciembre -> Invierno (UTC+1)
  if (month < 3 || month > 10) {
    return 1;
  }
  if (month > 3 && month < 10) {
    return 2;
  }

  // Lógica para los meses de cambio: Marzo y Octubre
  // tm_wday: 0=Domingo, 1=Lunes, ..., 6=Sábado
  struct tm timeinfo;
  timeinfo.tm_year = year - 1900;
  timeinfo.tm_mon = month - 1;

  // Encontrar el último domingo del mes
  timeinfo.tm_mday = 31; // Empezamos por el día 31
  mktime(&timeinfo); // Normaliza la fecha (ajusta si el mes tiene menos de 31 días)
  int lastSunday = timeinfo.tm_mday - timeinfo.tm_wday;

  if (month == 3) { // Cambio a horario de verano
    // Si el día actual es ANTES del último domingo, es invierno
    if (day < lastSunday) return 1;
    // Si es el último domingo pero ANTES de la 1:00 UTC, sigue siendo invierno
    if (day == lastSunday && hour < 1) return 1;
    // Si no, ya es verano
    return 2;
  }

  if (month == 10) { // Cambio a horario de invierno
    // Si el día actual es ANTES del último domingo, es verano
    if (day < lastSunday) return 2;
    // Si es el último domingo pero ANTES de la 1:00 UTC, sigue siendo verano
    if (day == lastSunday && hour < 1) return 2;
    // Si no, ya es invierno
    return 1;
  }
  
  return 1; // Por seguridad, valor por defecto
}