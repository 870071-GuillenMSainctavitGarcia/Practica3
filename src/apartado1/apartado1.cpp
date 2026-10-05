#include <WiFi.h>

extern "C" {
  #include "ping/ping_sock.h"
}

// Sustituir con datos de vuestra red
const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

void medirPing();

void setup()
{
  Serial.begin(115200);
  delay(10);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Conectando a:\t");
  Serial.println(ssid); 

  // Esperar a que nos conectemos
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(200);
  Serial.print('.');
  }

  // Mostrar mensaje de exito y dirección IP asignada
  Serial.println();
  Serial.print("Conectado a:\t");
  Serial.println(WiFi.SSID()); 
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());

  medirPing();
}

void loop() 
{
}

void medirPing() {
  Serial.println("\n--- Iniciando prueba de Ping a Google (8.8.8.8) ---");

  ip_addr_t target_addr;
  IP_ADDR4(&target_addr, 8, 8, 8, 8); // Servidor DNS de Google

  esp_ping_config_t config = ESP_PING_DEFAULT_CONFIG();
  config.target_addr = target_addr;
  config.count = 4;           // Se envían 4 paquetes
  config.interval_ms = 1000;  // 1 segundo entre envíos

  esp_ping_handle_t ping;

  if (esp_ping_new_session(&config, NULL, &ping) == ESP_OK) {
    esp_ping_start(ping);

    // Esperar a que terminen de enviarse los 4 paquetes
    delay(4500);

    // Extraer métricas de latencia y respuestas
    uint32_t tiempoTotalMs = 0;
    uint32_t respuestas = 0;

    esp_ping_get_profile(ping, ESP_PING_PROF_REPLY, &respuestas, sizeof(respuestas));
    esp_ping_get_profile(ping, ESP_PING_PROF_DURATION, &tiempoTotalMs, sizeof(tiempoTotalMs));

    if (respuestas > 0) {
      uint32_t latenciaPromedio = tiempoTotalMs / respuestas;
      Serial.print("¡Ping exitoso! Paquetes recibidos: ");
      Serial.print(respuestas);
      Serial.println("/4");
      
      Serial.print("Latencia promedio (Ping): ");
      Serial.print(latenciaPromedio);
      Serial.println(" ms");
    } else {
      Serial.println("Error: No hubo respuesta al Ping (Timeout).");
    }

    esp_ping_delete_session(ping);
  } else {
    Serial.println("Error al inicializar el módulo de Ping.");
  }
}



