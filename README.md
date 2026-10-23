 # Práctica 3: Conectividad Wi-Fi, Protocolos de Red (TCP, NTP, FTP, HTTP, MQTT) y Formatos IoT (ESP32-S3)

## Descripción General

Este proyecto implementa un conjunto de soluciones orientadas a la conectividad IoT y comunicación en red sobre el microcontrolador ESP32-S3. Se abordan desde pruebas de nivel de red (Ping e ICMP) hasta protocolos de aplicación como NTP para la sincronización temporal, Sockets TCP para streaming de datos bidireccional, Servidores Web asíncronos en HTTP con archivos embebidos (SPIFFS), transferencia remota de archivos vía FTP en formato SenML JSON y comunicación en la nube mediante el protocolo MQTT conectado a la plataforma Adafruit IO.


## ¿Como probar los subapartados del proyecto?

Como cada apartado es un código distinto:

1. Si vas a probar los Apartados 1 a 5, 7, 8, 9 o 10, abre el archivo de la carpeta correspondiente o ajusta el filtro build_src_filter en el archivo platformio.ini para apuntar al subapartado deseado.
2. Si vas a probar la comunicación TCP con script en Python (Apartado 6):
   * Carga src/apartado6/apartado6.cpp en la ESP32-S3.
   * Opcionalmente carga src/apartado6/apartado6Nano.cpp en la Arduino Nano 33 BLE si se usa la captura I2C real.
   * Ejecuta el script de servidor TCP apartado6b.py en tu ordenador usando el intérprete de Python.
3. Si vas a probar la interfaz web (Apartado 7), sube la imagen del sistema de archivos SPIFFS (index.html, style.css) mediante PlatformIO a la ESP32-S3.
4. Abre el Monitor Serie a 115200 baudios.


## Estructura del Proyecto

```text
Practica3/
├── data/                      # Archivos estáticos para el servidor Web (SPIFFS)
│   ├── index.html             # Interfaz de usuario con actualización AJAX
│   └── style.css              # Hoja de estilos para la interfaz web
├── src/
│   ├── apartado1_2/           # Diagnóstico ICMP (Ping) y verificación Wi-Fi
│   │   └── apartado1_2.cpp
│   ├── apartado2/             # Sincronización temporal con servidor NTP
│   │   └── apartado2.cpp
│   ├── apartado3/             # Cliente TCP básico para envío de datos
│   │   └── apartado3.cpp
│   ├── apartado4/             # Streaming continuo por Socket TCP con fecha NTP
│   │   └── apartado4.cpp
│   ├── apartado5/             # Control de flujo bidireccional (START/STOP) sobre TCP
│   │   └── apartado5.cpp
│   ├── apartado6/             # Sistema I2C a TCP / Servidor Python local
│   │   ├── apartado6.cpp      # Maestro ESP32-S3 y cliente TCP
│   │   ├── apartado6Nano.cpp  # Esclavo Arduino Nano 33 BLE
│   │   └── apartado6b.py      # Servidor Socket TCP en Python para PC
│   ├── apartado7/             # Servidor Web HTTP Asíncrono con SPIFFS
│   │   └── apartado7.cpp
│   ├── apartado8/             # Generación y serialización de tramas SenML JSON
│   │   └── apartado8.cpp
│   ├── apartado9/             # Cliente FTP para subida automatizada de archivos JSON
│   │   └── apartado9.cpp
│   └── apartado10/            # Publicación y suscripción MQTT en la nube (Adafruit IO)
│       └── apartado10.cpp
├── platformio.ini             # Entornos de compilación y monitorización de PlatformIO
└── README.md                  # Documentación principal
```

## APARTADO 1 y 2 (Ping e ICMP)

Procedimiento: Asociación de la ESP32-S3 a un punto de acceso Wi-Fi en modo estación (WIFI_STA) y ejecución de una prueba de latencia (Ping) por hardware hacia los servidores DNS de Google.

- **Lógica y Funciones Clave:**

    * WiFi.mode(WIFI_STA) / WiFi.begin(ssid, password): Configura la interfaz de red sin hilos en modo cliente e inicia la conexión con el router.
    * IP_ADDR4(&target_addr, 8, 8, 8, 8): Define la dirección de destino IPv4 objetivo (8.8.8.8).
    * esp_ping_config_t: Estructura de configuración del stack nativo ESP-IDF para definir el recuento de paquetes (4) e intervalo entre envíos (1000 ms).
    * esp_ping_new_session() / esp_ping_start(): Crea e inicia la sesión de eco ICMP por socket sin bloquear la pila principal.
    * esp_ping_get_profile(): Extrae los contadores de paquetes recibidos con éxito y calcula la latencia media global en milisegundos.

## APARTADO 2 (Sincronización NTP)

Procedimiento: Conexión a servidores de tiempo en internet (NTP) para obtener la hora UTC y sincronizar el reloj interno del sistema ajustado a la zona horaria europea.

- **Lógica y Funciones Clave:**

    * configTime(0, 0, ntpServer1, ntpServer2): Inicializa el cliente NTP registrando los servidores primario (pool.ntp.org) y secundario.
    * setenv("TZ", tzInfo, 1) / tzset(): Configura la variable de entorno del sistema con la cadena POSIX de zona horaria (CET-1CEST), gestionando el cambio de horario de verano e invierno.
    * getLocalTime(&timeinfo): Recupera la estructura tm con la fecha y hora desglosadas actualizadas desde el reloj interno.
    * strftime(): Formatea el timestamp en una cadena de texto legible (Día, Mes, Número, Año, HH:MM:SS).

## APARTADO 3

Procedimiento: Implementación de un cliente TCP utilizando la clase WiFiClient para entablar comunicación con un socket receptor en la PC (SocketTest) en un puerto determinado.

- **Lógica y Funciones Clave:**

    * WiFiClient client: Instancia un objeto de comunicación para la pila de sockets TCP/IP.
    * client.connect(hostPC, puerto): Lanza la petición de apertura de socket TCP con la dirección IP de la PC y el puerto servidor (8888).
    * client.println(): Transmite tramas de texto sobre la conexión orientada a flujo de datos.
    * client.stop(): Cierra formalmente la sesión del socket liberando el búfer del controlador.

## APARTADO 4

Procedimiento: Integración del servicio NTP con la conexión TCP para enviar de forma continua la hora local actualizada cada segundo a la aplicación SocketTest.

- **Lógica y Funciones Clave:**

    * millis() - ultimoEnvio >= 1000: Temporización no bloqueante para gestionar la tasa de emisión sin detener el procesamiento de la red.
    * client.connected(): Verifica en cada ciclo del bucle si el enlace del socket TCP continúa activo, forzando la reconexión en caso de caída.
    * client.available() / client.readStringUntil('\n'): Escucha lecturas en el búfer de entrada TCP para recibir confirmaciones de texto enviadas desde el servidor.

## APARTADO 5

Procedimiento: Control remoto de flujo bidireccional sobre la conexión TCP mediante órdenes de texto enviadas desde la PC para pausar o reanudar el envío de información.

- **Lógica y Funciones Clave:**

    * bool flagStop: Variable de estado global que inhabilita o habilita el empaquetado y transmisión de la hora.
    * respuestaPC.trim(): Elimina caracteres de control y espacios en blanco de las cadenas recibidas por el socket.
    * Parsing de comandos: Evalúa la presencia de las palabras clave START / start (asigna flagStop = false) y STOP / stop (asigna flagStop = true).
    * client.println() condicional: Únicamente ejecuta la transmisión por red si el flag de parada se encuentra desactivado.

## APARTADO 6 - Maestro (ESP32-S3) y Servidor Python

Procedimiento: Captura de tramas de aceleración (vía I2C o simulación) y retransmisión inalámbrica en bloques a un servidor Socket TCP en Python ejecutado en la PC.

- **Lógica y Funciones Clave:**

    * apartado6.cpp (ESP32-S3): Adquiere 10 muestras vectoriales (X, Y, Z) consecutivas, enciende un LED como testigo visual de transmisión y las envía formateadas en un único paquete TCP.
    * apartado6b.py (Script Python en PC):
        * socket.socket(socket.AF_INET, socket.SOCK_STREAM): Declara un socket IPv4 TCP nativo.
        * server_socket.bind(('0.0.0.0', 8888)) / listen(1): Escucha en todas las interfaces de red del ordenador.
        * conn.recv(1024): Recibe e interpreta las ráfagas de bytes enviadas por el ESP32-S3 e imprime la salida en la consola de la PC.

## APARTADO 6 - Esclavo (Arduino Nano 33 BLE)

Procedimiento: Muestreo ininterrumpido de la IMU integrada LSM9DS1 y respuesta a solicitudes I2C del ESP32-S3.

- **Lógica y Funciones Clave:**

    * IMU.readAcceleration(): Lee las componentes de aceleración en unidades gravitacionales (g).
    * Wire.onRequest(responderPeticionI2C): Interrupción de hardware que sirve la estructura DatosAcel al maestro en el bus I2C.
    * noInterrupts() / interrupts(): Aísla la copia de datos a la variable volatile para garantizar atomicidad ante accesos concurrentes.

## APARTADO 7

Procedimiento: Despliegue de un servidor Web HTTP asíncrono sobre la ESP32-S3 sirviendo archivos estáticos (HTML/CSS) desde la memoria Flash mediante el sistema de archivos SPIFFS, con actualización de datos en tiempo real mediante AJAX.

- **Lógica y Funciones Clave:**

    * SPIFFS.begin(true): Monta la partición de archivos SPIFFS en la memoria interna.
    * AsyncWebServer server(80): Instancia el servidor HTTP asíncrono escuchando en el puerto 80.
    * server.on("/", ...) / server.on("/style.css", ...): Define las rutas para entregar la página web y los estilos visuales.
    * processor: Función plantilla que reemplaza dinámicamente placeholders como %TIME% en la carga inicial.
    * Ruta /time: Endpoint ligero que responde con el tiempo transcurrido en texto plano (text/plain), consumido cada segundo por una petición asíncrona (JS/AJAX) en el navegador sin recargar la página.
    * Ruta /reset: Endpoint que reinicia el contador de tiempo global (offsetMillis = millis()).

## APARTADO 8

Procedimiento: Generación y formateo de datos de sensores siguiendo la especificación estándar industrial SenML JSON (RFC 8428) mediante la librería ArduinoJson.

- **Lógica y Funciones Clave:**

    * JsonDocument doc / doc.to<JsonArray>(): Crea la estructura raíz en formato lista (Array) requerida por la norma SenML.
    * Atributos estándar SenML:
        * bn (Base Name): Identificador único del dispositivo (urn:dev:esp32-s3:temp-sensor-01).
        * bt (Base Time): Marca de tiempo relativa en segundos.
        * bu (Base Unit): Unidad de medida reglamentada (Cel para grados Celsius).
        * n (Name) y v (Value): Nombre de la variable (temperature) y valor flotante cuantificado.
    * serializeJson(doc, jsonOutput): Convierte el árbol de datos estructurado en una cadena de texto JSON comprimida.

## APARTADO 9

Procedimiento: Automatización de transferencia remota de archivos que genera periódicamente documentos SenML JSON y los sube mediante protocolo FTP a un servidor alojado en la red local.

- **Lógica y Funciones Clave:**

    * ESP32_FTPClient ftp(...): Configura las credenciales de conexión, IP del servidor FTP local, puerto (21) y tiempos de espera.
    * Nombramiento dinámico: Genera nombres de archivo únicos con el formato grupo01_HHMMSS.json utilizando la función snprintf().
    * ftp.OpenConnection() / ftp.CloseConnection(): Establece y cierra la sesión de control FTP.
    * ftp.InitFile("Type I"): Selecciona el modo de transferencia en binario.
    * ftp.NewFile() / ftp.Write(): Crea el archivo en el directorio remoto e inserta el contenido JSON serializado.

## APARTADO 10

Procedimiento: Conexión de la ESP32-S3 a un Broker MQTT en la nube (Adafruit IO) para publicar datos telemétricos periódicos y recibir datos mediante suscripción en tiempo real.

- **Lógica y Funciones Clave:**

    * EspMQTTClient client(...): Configura la conexión unificada Wi-Fi y MQTT hacia el host io.adafruit.com en el puerto 1883 con la clave AIO Key.
    * onConnectionEstablished(): Callback automático ejecutado tras autenticarse con éxito en el broker.
    * client.subscribe(topic, callback): Se suscribe al canal (feed) de Adafruit IO (<username>/feeds/<feed-key>) para procesar datos entrantes de forma asíncrona.
    * client.publish(topic, valor): Publica valores simulados de sensores en la nube cada 15 segundos.
    * client.loop(): Mantiene la conexión, procesa los pings de keep-alive y gestiona la cola de mensajes MQTT entrantes y salientes.