#include <WiFi.h>

// 1. CONFIGURACIÓN DE RED
const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";   // Contraseña de tu red Wi-Fi

// 2. CONFIGURACIÓN DEL SERVIDOR TCP (SocketTest en la PC)
const char* hostPC   = "10.143.42.92";      // Coloca aquí la Dirección IPv4 de tu PC
const uint16_t puerto = 8888;              // Puerto configurado en SocketTest

WiFiClient client;

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n=== CHAT SOCKET TCP: ESP32-S3 <-> PC ===");

  // Conexión Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a la red Wi-Fi");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n¡Wi-Fi Conectado!");
  Serial.print("IP local de la ESP32-S3: ");
  Serial.println(WiFi.localIP());

  // Intentar conectar con SocketTest
  Serial.print("Conectando al servidor SocketTest en ");
  Serial.print(hostPC);
  Serial.print(":");
  Serial.println(puerto);

  while (!client.connect(hostPC, puerto)) {
    Serial.println("Fallo de conexión con SocketTest. Reintentando en 3 segundos...");
    delay(3000);
  }

  Serial.println("\n>>> ¡CONEXIÓN ESTABLECIDA CON SOCKETTEST! <<<");
  Serial.println("Puedes escribir un mensaje en el Monitor Serie y pulsar ENTER para enviarlo.\n");
}

void loop() {
  // Verificación de estado de la conexión
  if (!client.connected()) {
    Serial.println("\n[AVISO] Se ha perdido la conexión con SocketTest. Intentando reconectar...");
    if (client.connect(hostPC, puerto)) {
      Serial.println("[ÉXITO] Reconectado a SocketTest.");
    }
    delay(2000);
    return;
  }

  // --- RECEPTION: Escuchar mensajes entrantes desde SocketTest ---
  while (client.available()) {
    String mensajeRecibido = client.readStringUntil('\n');
    Serial.print("[PC -> ESP32]: ");
    Serial.println(mensajeRecibido);
  }

  // --- TRANSMISION: Enviar mensajes escritos en el Monitor Serie de Arduino ---
  if (Serial.available() > 0) {
    String mensajeAEnviar = Serial.readStringUntil('\n');
    mensajeAEnviar.trim(); // Eliminar saltos de línea o espacios innecesarios

    if (mensajeAEnviar.length() > 0) {
      client.println(mensajeAEnviar); // Enviar mensaje por Socket TCP
      Serial.print("[ESP32 -> PC]: ");
      Serial.println(mensajeAEnviar);
    }
  }

  vTaskDelay(pdMS_TO_TICKS(10)); // Pausa corta para no saturar el micro
}