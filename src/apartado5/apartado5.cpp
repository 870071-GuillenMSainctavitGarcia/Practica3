#include <WiFi.h>
#include "time.h"

// 1. DATOS DE TU RED WI-FI
const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

// 2. DATOS DEL SERVIDOR SOCKETTEST (En la PC)
const char* hostPC   = "10.143.42.92"; // Cambia por la IPv4 actual de tu PC
const uint16_t puerto = 8888;

// 3. CONFIGURACIÓN NTP (Hora de España / UTC+1 con cambio de horario)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;       // UTC+1 (3600 segundos)
const int   daylightOffset_sec = 3600;  // Horario de verano (+1 hora adicional)

bool flagStop = true;

WiFiClient client;
unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conexión a la red Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado a la red Wi-Fi!");

  // Configurar e iniciar la hora desde el servidor NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  struct tm timeinfo;
  Serial.print("Obteniendo hora local por NTP");
  while (!getLocalTime(&timeinfo)) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n¡Hora sincronizada con éxito!");

  // Conectar con el servidor TCP en la PC (SocketTest)
  Serial.print("Conectando a SocketTest en ");
  Serial.print(hostPC);
  Serial.print(":");
  Serial.println(puerto);

  while (!client.connect(hostPC, puerto)) {
    Serial.println("Fallo al conectar con SocketTest. Reintentando en 2s...");
    delay(2000);
  }
  Serial.println(">>> Conectado a SocketTest correctamente <<<");
}

void loop() {
  // Verificar si la conexión con SocketTest sigue activa
  if (!client.connected()) {
    Serial.println("Conexión perdida. Intentando reconectar...");
    if (client.connect(hostPC, puerto)) {
      Serial.println("Reconectado a SocketTest.");
    }
    delay(1000);
    return;
  }

  // Enviar la hora local cada 1 segundo (1000 ms)
  if (millis() - ultimoEnvio >= 1000) {
    ultimoEnvio = millis();

    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char horaFormateada[64];
      // Formato: HH:MM:SS (ejemplo: 22:45:33)
      if(!flagStop){
        strftime(horaFormateada, sizeof(horaFormateada), "Hora Local ESP32: %H:%M:%S", &timeinfo);

        // Enviar trama de texto por el Socket TCP a la PC
        client.println(horaFormateada);

        // Mostrar por el Monitor Serie para verificación
        Serial.print("[Enviado por TCP]: ");
        Serial.println(horaFormateada);
      }
      else{
        Serial.println("Envío de datos detenido.");
      }
      
    }
  }

  // Escuchar por si SocketTest envía alguna respuesta
  while (client.available()) {
    String respuestaPC = client.readStringUntil('\n');
    respuestaPC.trim();

    if(respuestaPC=="stop" || respuestaPC=="STOP"){
        Serial.println("Se ha recibido la orden de parada. Deteniendo el envío de datos.");
        flagStop=true;

    }
    
     if(respuestaPC=="start" || respuestaPC=="START"){
        Serial.println("Se ha recibido la orden de inicio. Reanudando el envío de datos.");
        flagStop=false;

    }

    if (respuestaPC!="stop" && respuestaPC!="STOP" && respuestaPC!="start" && respuestaPC!="START"){
        Serial.println("Se ha recibido un mensaje desconocido. No se realizará ninguna acción.");
    }
    Serial.print("[Recibido desde PC]: ");
    Serial.println(respuestaPC);
  }
}