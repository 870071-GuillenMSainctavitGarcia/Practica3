#include <WiFi.h>

const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

// IP de tu ordenador (donde estará abierto SocketTest)
const char* hostPC   = "10.143.42.92"; 
const uint16_t puerto = 8888;

WiFiClient client;

void setup() {
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi Conectado!");
}

void loop() {
  // Intentar conectar con el servidor SocketTest en la PC
  if (client.connect(hostPC, puerto)) {
    Serial.println("Conectado a SocketTest en la PC!");
    
    // Enviar un mensaje de prueba
    client.println("Hola desde la ESP32-S3!");
   
    // Desconectar tras el envío
    client.stop();
  } else {
    Serial.println("Fallo al conectar con SocketTest. Verifica la IP y el puerto.");
  }

  delay(5000); // Esperar 5 segundos antes del siguiente envío
}