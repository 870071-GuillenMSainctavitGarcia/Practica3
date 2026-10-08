#include "WiFi.h"
#include "ESPAsyncWebServer.h"
#include "SPIFFS.h"

const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

AsyncWebServer server(80);

unsigned long offsetMillis = 0;

String getFormattedTime() {
  unsigned long currentMillis = millis() - offsetMillis;
  
  unsigned long totalSeconds = currentMillis / 1000;
  unsigned long seconds = totalSeconds % 60;
  unsigned long minutes = (totalSeconds / 60) % 60;
  unsigned long hours   = (totalSeconds / 3600) % 24;

  char timeBuffer[10];
  snprintf(timeBuffer, sizeof(timeBuffer), "%02lu:%02lu:%02lu", hours, minutes, seconds);
  return String(timeBuffer);
}

String processor(const String& var) {
  if (var == "TIME") {
    return getFormattedTime();
  }
  return String();
}

void setup() {
  Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi..");
  }

  Serial.print("IP del ESP32: ");
  Serial.println(WiFi.localIP());

  // Ruta principal HTML
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  // Ruta CSS
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/style.css", "text/css");
  });

  // Ruta AJAX para obtener solo la hora en texto plano (cada 1s)
  server.on("/time", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", getFormattedTime());
  });

  // Ruta para resetear a las 00:00:00
  server.on("/reset", HTTP_GET, [](AsyncWebServerRequest *request) {
    offsetMillis = millis();
    request->send(SPIFFS, "/index.html", String(), false, processor);
  });

  server.begin();
}

void loop() {
}