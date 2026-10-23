#include <Arduino.h>
#include "EspMQTTClient.h"

// Parámetros de red y servidor Adafruit IO
#define WIFI_SSID       "Redmi Note 9"
#define WIFI_PASS       "contrasena"

//SIN DEFINIR PARA EL USUARIO Y CONTRASEÑA DE ADAFRUIT IO, YA QUE ESTO ES SENSIBLE Y NO SE DEBE COMPARTIR PÚBLICAMENTE

// Inicialización de la librería EspMQTTClient
// FALTA LA LINEA DEL ESP32_MQTTCLIENT PARA DEFINIR EL USUARIO Y CONTRASEÑA DE ADAFRUIT IO

unsigned long lastMsgTime = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando ESP32-S3 con MQTT...");
  
  // Opcional: Activar mensajes de depuración en la consola
  client.enableDebuggingMessages();
}

// Se ejecuta de manera automática cada vez que se establece conexión con el broker MQTT
void onConnectionEstablished() { 
  Serial.println("\n¡CONECTADO CON ÉXITO A ADAFRUIT IO!");

  // Suscripción al feed para recibir cambios en tiempo real
  // Formato del topic en Adafruit: <username>/feeds/<feed-key>
  String topicSuscripcion = String(AIO_USERNAME) + "/feeds/" + AIO_FEED_KEY; // EN ESTA LÍNEA SE CONSTRUYE EL TOPIC DE SUSCRIPCIÓN AL FEED DE ADAFRUIT IO
  
  client.subscribe(topicSuscripcion, [](const String & payload) { // Callback que se ejecuta cada vez que se recibe un mensaje en el feed suscrito
    Serial.println("==========================================");
    Serial.print("-> ¡NUEVO DATO RECIBIDO DESDE LA NUBE!: ");
    Serial.println(payload); // PAYLOAD ES EL DATO QUE SE RECIBE DESDE EL FEED DE ADAFRUIT IO
    Serial.println("==========================================");
  });

  Serial.print("Suscrito al topic: ");
  Serial.println(topicSuscripcion);
}

void loop() {
  // client.loop() procesa las señales de red y mantiene la conexión MQTT
  client.loop(); //REVISA SI HAY DATOS NUEVOS DEL BROKER Y EJECUTA LOS CALLBACKS CORRESPONDIENTES

  // Enviar datos al feed automáticamente cada 15 segundos
  if (millis() - lastMsgTime > 15000) {
    lastMsgTime = millis();

    float valorSimulado = 20.0 + (random(0, 100) / 10.0); // Valor aleatorio entre 20.0 y 30.0
    String topicPublicacion = String(AIO_USERNAME) + "/feeds/" + AIO_FEED_KEY;

    Serial.print("Publicando valor por MQTT: ");
    Serial.println(valorSimulado);

    client.publish(topicPublicacion, String(valorSimulado));
  }
}