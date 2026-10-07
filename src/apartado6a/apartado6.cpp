#include <Arduino.h>
#include <Wire.h>
#include <WiFi.h>

// 1. CONFIGURACIÓN DE RED Y SERVIDOR TCP
const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

const char* hostPC   = "10.143.42.92"; // IPv4 de tu PC (SocketTest)
const uint16_t puerto = 8888;            // Puerto de SocketTest

// 2. HARDWARE I2C Y LED
#define DIRECCION_ESCLAVO 0x08
#define PIN_SDA 8
#define PIN_SCL 9
#define PIN_LED 10

// Estructura de datos del acelerómetro
struct DatosAcel {
  float ax, ay, az;
};

WiFiClient client;
DatosAcel paquete[10];

void setup() {
  Serial.begin(115200);
  pinMode(PIN_LED, OUTPUT);
  digitalWrite(PIN_LED, LOW);

  // Inicializar I2C Maestro (SDA: 8, SCL: 9)
  Wire.setPins(PIN_SDA, PIN_SCL);
  Wire.begin();

  // Conectar a la red Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Wi-Fi Conectado!");

  // Conectar con el Servidor TCP en la PC
  Serial.print("Conectando al Servidor TCP...");
  while (!client.connect(hostPC, puerto)) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("\n¡Conectado al Servidor TCP (SocketTest)!");
}

void loop() {
  // Asegurar que la conexión TCP siga activa
  if (!client.connected()) {
    Serial.println("Conexión perdida. Intentando reconectar...");
    if (client.connect(hostPC, puerto)) {
      Serial.println("Reconectado a SocketTest.");
    }
    delay(1000);
    return;
  }

  // 1. Obtener 10 muestras consecutivas desde la Nano por I2C (100 ms entre cada una)
  // 1. Obtener 10 muestras consecutivas (100 ms entre cada una = 1 segundo)
  for (int i = 0; i < 10; i++) {
    
    // =========================================================================
    // MODIFICA AQUÍ CUANDO TENGAS LA NANO CONECTADA:
    // =========================================================================
    
    // --- [OPCIÓN SIMULACIÓN (ACTIVADA HOY)]: ---
    paquete[i].ax = (random(-100, 100) / 100.0); // Valores entre -1.00g y 1.00g
    paquete[i].ay = (random(-100, 100) / 100.0);
    paquete[i].az = 0.98 + (random(-10, 10) / 100.0); // Alrededor de 1.00g (gravedad)

    /* 
    // --- [OPCIÓN I2C REAL (DESACTIVADA PARA HOY)]: ---
    // Descomenta este bloque y borra las 3 líneas de arriba para usar la Nano:
    
    uint8_t bytesRecibidos = Wire.requestFrom(DIRECCION_ESCLAVO, sizeof(DatosAcel));
    if (bytesRecibidos == sizeof(DatosAcel)) {
      Wire.readBytes((uint8_t*)&paquete[i], sizeof(DatosAcel));
    } else {
      while (Wire.available()) Wire.read();
      paquete[i].ax = 0; paquete[i].ay = 0; paquete[i].az = 0;
    }
    */
    // =========================================================================

    delay(100); 
  }

  // 2. Encender el LED
  digitalWrite(PIN_LED, HIGH);

  // 3. Transmitir el paquete de 10 muestras por el Socket TCP a la PC
  client.println("\n--- [PAQUETE 1 SEG - 10 MUESTRAS SOLICITADAS A LA NANO] ---");
  for (int i = 0; i < 10; i++) {
    client.print("Muestra "); client.print(i + 1);
    client.print(" | AccX: "); client.print(paquete[i].ax, 2);
    client.print(" g | AccY: "); client.print(paquete[i].ay, 2);
    client.print(" g | AccZ: "); client.print(paquete[i].az, 2);
    client.println(" g");
  }
  client.println("------------------------------------------------------------");

  // 4. Apagar el LED tras 200 ms
  delay(200);
  digitalWrite(PIN_LED, LOW);
}