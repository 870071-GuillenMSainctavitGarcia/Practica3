#include <Arduino.h>
#include <ArduinoJson.h>

unsigned long previousMillis = 0;
const long interval = 10000; // 10 segundos (10000 ms)

void setup() {
  Serial.begin(115200);
}

void generateSenMLJson() {
  // Crear el documento JSON (compatible con ArduinoJson v6 y v7)
  JsonDocument doc;

  // SenML requiere un Array en la raíz
  JsonArray root = doc.to<JsonArray>();

  // Crear el registro/medida principal
  JsonObject record = root.add<JsonObject>();

  // Campos estándar SenML (RFC 8428)
  record["bn"] = "urn:dev:esp32-s3:temp-sensor-01"; // Base Name (Identificador)
  record["bt"] = millis() / 1000;                     // Base Time (Marca temporal en segundos)
  record["bu"] = "Cel";                              // Base Unit (Grados Celsius en SenML)
  record["n"]  = "temperature";                      // Name
  
  // Generar un valor de temperatura simulado entre 20.0 °C y 30.0 °C
  float dummyTemp = 20.0 + (random(0, 100) / 10.0);
  record["v"] = dummyTemp;                           // Value

  // Serializar e imprimir por el puerto serie
  String jsonOutput;
  serializeJson(doc, jsonOutput);

  Serial.println("--- Mensaje SenML JSON generado ---");
  Serial.println(jsonOutput);
}

void loop() {
  unsigned long currentMillis = millis();

  // Ejecuta la función cada 10 segundos sin bloquear el loop
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    generateSenMLJson();
  }
}