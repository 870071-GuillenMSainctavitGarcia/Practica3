#include <WiFi.h>
#include "time.h"

// 1. DATOS DE TU RED WI-FI
const char* ssid     = "Redmi Note 9";
const char* password = "contrasena";

// 2. DATOS DEL SERVIDOR SOCKETTEST (En la PC)
const char* hostPC   = "10.143.42.92"; //  IPv4 actual de tu PC
const uint16_t puerto = 8888; // Eligo un puerto que esté libre en la PC y que coincida con el configurado en SocketTest

// 3. CONFIGURACIÓN NTP (Hora de España / UTC+1 con cambio de horario)
const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 3600;       // UTC+1 (3600 segundos)
const int   daylightOffset_sec = 3600;  // Horario de verano (+1 hora adicional)
// FLAG PARA CONTROLAR EL ENVÍO DE DATOS
bool flagStop = true;

// SE CREA EL OBJETO CLIENTE TCP PARA LA CONEXIÓN CON SOCKETTEST
WiFiClient client;
unsigned long ultimoEnvio = 0;

void setup() {
  Serial.begin(115200);
  delay(1000);

  // Conexión a la red Wi-Fi
  // SE ELIGE WIFI_STA (modo estación) para conectarse a un router o punto de acceso
  WiFi.mode(WIFI_STA); // EL ESP32 actuará como cliente Wi-Fi
  WiFi.begin(ssid, password);
  Serial.print("Conectando a Wi-Fi");
  //Mientras que no se conecte, se mostrará un punto cada 500 ms
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n¡Conectado a la red Wi-Fi!");
  // CONFIGTIME PERTENECE A LA LIBRERÍA <TIME.H> Y SE USA PARA CONFIGURAR EL SERVIDOR NTP Y OBTENER LA HORA LOCAL
  // Configurar e iniciar la hora desde el servidor NTP
  // Ofset segun zona horaria + offset por verano + servidos NTP ELEGIDO
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
  // defino una estructura para almacenar la hora local
  struct tm timeinfo;
  Serial.print("Obteniendo hora local por NTP");
  while (!getLocalTime(&timeinfo)) { //Hasta que no se obtenga la hora, se mostrará un punto cada 500 ms
    Serial.print(".");
    delay(500);
  }
  Serial.println("\n¡Hora sincronizada con éxito!");

  // Conectar con el servidor TCP en la PC (SocketTest)
  Serial.print("Conectando a SocketTest en ");
  Serial.print(hostPC);
  Serial.print(":");
  Serial.println(puerto);

// EL PC ES EL SERVIDOR Y EL ESP32 ES EL CLIENTE, POR ESO SE USA client.connect()
  while (!client.connect(hostPC, puerto)) {
    Serial.println("Fallo al conectar con SocketTest. Reintentando en 2s...");
    delay(2000);
  }
  Serial.println(">>> Conectado a SocketTest correctamente <<<");
}

void loop() {
  // Verificar si la conexión con SocketTest sigue activa
  // Se verifica si fucniona la conexión TCP, si no, se intenta reconectar
  if (!client.connected()) {
    Serial.println("Conexión perdida. Intentando reconectar...");
    if (client.connect(hostPC, puerto)) {  // se lanza un intento de reconexión
      Serial.println("Reconectado a SocketTest.");
    }
    delay(1000);
    return;
  }

  // Enviar la hora local cada 1 segundo (1000 ms)
  if (millis() - ultimoEnvio >= 1000) { // si la resta del timer  y el ultimo envío es mayor o igual a 1000 ms, se envía la hora local
    ultimoEnvio = millis();

    // Obtener la hora local actual
    struct tm timeinfo;
    if (getLocalTime(&timeinfo)) {
      char horaFormateada[64]; // se crea un buffer para almacenar la hora formateada
      // Formato: HH:MM:SS (ejemplo: 22:45:33)
      // Al estar el flagStop en true, se detiene el envío de datos. Si está en false, se envía la hora local al servidor TCP (SocketTest)
      if(!flagStop){
        strftime(horaFormateada, sizeof(horaFormateada), "Hora Local ESP32: %H:%M:%S", &timeinfo);

        // Enviar trama de texto por el Socket TCP a la PC
        client.println(horaFormateada);

        // Mostrar por el Monitor Serie para verificación
        Serial.print("[Enviado por TCP]: ");
        Serial.println(horaFormateada);
        // La muestro tanto por el Monitor Serie como por el Socket TCP.
      }
      else{
        Serial.println("Envío de datos detenido.");
      }
      
    }
  }

  // Escuchar por si SocketTest envía alguna respuesta
  while (client.available()) { // EL CLIENT.AVAILABLE DEVUELVE EL NÚMERO DE BYTES DISPONIBLES PARA LEER DESDE EL SOCKET TCP. SI HAY DATOS DISPONIBLES, ENTRA EN EL WHILE.
    // Mientras haya datos disponibles para leer desde el servidor TCP (SocketTest)
    String respuestaPC = client.readStringUntil('\n'); // Se lee la respuesta hasta encontrar un salto de línea
    respuestaPC.trim(); // SE ELIMINAN ESPACIOS EN BLANCO AL PRINCIPIO Y AL FINAL DE LA CADENA
    // SI ES STOP ENTRA EN EL IF Y SE PONE EL FLAG EN TRUE.
    if(respuestaPC=="stop" || respuestaPC=="STOP"){
        Serial.println("Se ha recibido la orden de parada. Deteniendo el envío de datos.");
        flagStop=true;

    }
    // SI ES START ENTRA EN EL IF Y SE PONE EL FLAG EN FALSE.
     if(respuestaPC=="start" || respuestaPC=="START"){
        Serial.println("Se ha recibido la orden de inicio. Reanudando el envío de datos.");
        flagStop=false;

    }
    // SI NO ES NI STOP NI START, ENTRA EN EL ELSE Y SE MUESTRA UN MENSAJE DE ERROR.
    if (respuestaPC!="stop" && respuestaPC!="STOP" && respuestaPC!="start" && respuestaPC!="START"){
        Serial.println("Se ha recibido un mensaje desconocido. No se realizará ninguna acción.");
    }
    // Mostrar la respuesta recibida desde la PC por el Socket TCP
    Serial.print("[Recibido desde PC]: ");
    Serial.println(respuestaPC);
  }
}