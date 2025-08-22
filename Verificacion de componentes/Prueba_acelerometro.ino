// Incluir las librerías necesarias
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Crear un objeto para el sensor MPU6050
Adafruit_MPU6050 mpu;

void setup() {
  // Inicializar la comunicación serie a 115200 baudios
  Serial.begin(115200);

  // Esperar a que el puerto serie se conecte
  while (!Serial) {
    delay(10);
  }

  Serial.println("Buscando dispositivo MPU6050...");

  // Inicializar el sensor MPU6050
  if (!mpu.begin()) {
    Serial.println("¡Error! No se encontró el sensor MPU6050. Revisa las conexiones.");
    while (1) {
      delay(10);
    }
  }
  
  Serial.println("¡Dispositivo MPU6050 encontrado!");

  // Opcional: Configurar los rangos del sensor (puedes descomentar para cambiarlos)
  // mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
  // mpu.setGyroRange(MPU6050_RANGE_500_DEG);
  // mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void loop() {
  // Crear objetos "evento" para almacenar las lecturas del sensor
  sensors_event_t a, g, temp;

  // Obtener todas las lecturas del sensor de una vez
  mpu.getEvent(&a, &g, &temp);

  // --- Imprimir valores del Acelerómetro ---
  Serial.print("Acc| X: ");
  Serial.print(a.acceleration.x);
  Serial.print("  Y: ");
  Serial.print(a.acceleration.y);
  Serial.print("  Z: ");
  Serial.print(a.acceleration.z);
  Serial.print(" m/s^2");

  // --- Imprimir valores del Giroscopio ---
  Serial.print("   |   Gyr| X: ");
  Serial.print(g.gyro.x);
  Serial.print("  Y: ");
  Serial.print(g.gyro.y);
  Serial.print("  Z: ");
  Serial.print(g.gyro.z);
  Serial.print(" rad/s");

  // --- Imprimir valor de la Temperatura ---
  Serial.print("   |   Temp: ");
  Serial.print(temp.temperature);
  Serial.println(" °C");

  // Esperar un poco antes de la siguiente lectura para no saturar el monitor
  delay(500);
}