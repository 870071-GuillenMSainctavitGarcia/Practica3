#include "WiFi.h" // libreria para manejar la conexión WiFi
#include "ESPAsyncWebServer.h" // libreria para crear un servidor web asíncrono
#include "SPIFFS.h" // libreria para manejar el sistema de archivos SPIFFS

//Wifi y contraseña de la red WiFi a la que se conectará el ESP32 y con la que se comunicará con la PC 
const char* ssid     = "Redmi Note 9";
const char* password = "contrasena";

AsyncWebServer server(80); // se crea un objeto de la clase AsyncWebServer que escucha en el puerto 80 (HTTP)
//Se eloge el puerto 80 porque es el puerto por defecto para HTTP y es el que se usará para la comunicación entre el ESP32 y la PC a través del navegador web.
unsigned long offsetMillis = 0;

//Función para obtener la hora formateada en el formato HH:MM:SS
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
// Función para procesar las variables en el HTML y reemplazarlas con su valor correspondiente
String processor(const String& var) {
  if (var == "TIME") {
    return getFormattedTime();
  }
  return String();
}
// Inicialización del servidor web y configuración de las rutas para servir los archivos HTML, CSS y AJAX
void setup() {
  Serial.begin(115200);
  // Inicializar SPIFFS para poder servir archivos desde el sistema de archivos del ESP32
  if (!SPIFFS.begin(true)) {
    Serial.println("Error al montar SPIFFS");
    return;
  }
  // Conectar a la red WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Conectando a WiFi..");
  }
  // Mostrar la IP asignada al ESP32 por el router
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
  // Iniciar el servidor web
  server.begin();
}

void loop() {
}