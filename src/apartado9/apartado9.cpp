#include <WiFi.h>
#include <ArduinoJson.h>
#include <ESP32_FTPClient.h>

// Credenciales Wi-Fi
const char* ssid     = "Redmi Note 9";
const char* password = "contrasena";

// Credenciales FTP (datos de FileZilla o del laboratorio)
char ftp_server[] = ""; // Dirección IP de tu PC en la red local
char ftp_user[]   = "grupo01";
char ftp_pass[]   = "";

ESP32_FTPClient ftp(ftp_server, ftp_user, ftp_pass, 5000, 2); // EL ESP32 ES EL CLIENTE Y LE DOY LA INFORMACIÓN DEL SERVIDOR FTP, USUARIO, CONTRASEÑA, TIEMPO DE ESPERA Y NIVEL DE VERBOSIDAD

unsigned long previousMillis = 0; // VARIABLES DE TIEMPO
const long interval = 10000; // 10 segundos

void setup() {
  Serial.begin(115200);

  WiFi.begin(ssid, password); // SI ESTA CONECTADO OK
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConectado a WiFi");
}

void uploadJsonFTP() { // FUNCION PARA SUBIR EL ARCHIVO JSON AL SERVIDOR FTP
  // 1. Obtener tiempo transcurrido para formatear ddmmss
  unsigned long totalSeconds = millis() / 1000;
  unsigned long seconds = totalSeconds % 60;
  unsigned long minutes = (totalSeconds / 60) % 60;
  unsigned long hours   = (totalSeconds / 3600) % 24;

  // Reemplaza "01" por el número real de tu grupo
  char fileName[30]; // Nombre del archivo JSON con formato "grupo01_hhmmss.json"
  snprintf(fileName, sizeof(fileName), "grupo01_%02lu%02lu%02lu.json", hours, minutes, seconds); // Formato del nombre del archivo: grupo01_hhmmss.json

  // 2. Generar SenML JSON
  JsonDocument doc;
  JsonArray root = doc.to<JsonArray>();
  JsonObject record = root.add<JsonObject>();
 //FORMATO DE LOS DATOS QUE SE VAN A SUBIR AL SERVIDOR FTP
  record["bn"] = "urn:dev:esp32-s3:temp-sensor-01";
  record["bt"] = totalSeconds;
  record["bu"] = "Cel";
  record["n"]  = "temperature";
  record["v"]  = 20.0 + (random(0, 100) / 10.0);

  String jsonOutput;
  serializeJson(doc, jsonOutput);

  // 3. Subir archivo mediante FTP
  Serial.print("Conectando al servidor FTP y subiendo ");
  Serial.println(fileName);

  ftp.OpenConnection();
  ftp.InitFile("Type I"); // Modo binario
  ftp.NewFile(fileName);
  ftp.Write(jsonOutput.c_str());
  ftp.CloseFile();
  ftp.CloseConnection();

  Serial.println("¡Archivo subido con éxito!");
}

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    uploadJsonFTP();
  }
}