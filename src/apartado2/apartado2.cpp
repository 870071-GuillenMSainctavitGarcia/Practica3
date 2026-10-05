// Libraries to get time from NTP Server
#include <WiFi.h>
#include <time.h>

// Sustituir con datos de vuestra red
const char* ssid     = "Redmi Note 9";
const char* password = "dcb324150aa7";

// NTP servers used to get the current time from the internet
const char* ntpServer1 = "pool.ntp.org";
const char* ntpServer2 = "time.nist.gov";

// Time zone string
// Example below is for US Eastern Time with automatic daylight saving
// Change this for your region
// See list of timezone strings https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
const char* tzInfo = "CET-1CEST,M3.5.0,M10.5.0/3"; // "Europe/Madrid","CET-1CEST,M3.5.0,M10.5.0/3"

void connectWiFi() {
  // Put Wi-Fi in station mode so ESP32 connects to a router
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // Wait until Wi-Fi connection is established
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

void syncTime() {
  // Start NTP using the two servers above
  configTime(0, 0, ntpServer1, ntpServer2);

  // Set the timezone for your region
  setenv("TZ", tzInfo, 1);
  tzset();

  // Wait until a valid time is received from the NTP server
  // 1577836800 is the Unix time for Jan 1, 2020
  time_t now = 0;
  while (time(&now) < 1577836800) {
    delay(500);
  }
}

// Function that prints formatted date and time
void printDateTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo, 2000)) {
    Serial.println("Failed to obtain time");
    return;
  }
  char formattedTime[80];  // Buffer to store the formatted string
  strftime(formattedTime, sizeof(formattedTime), "%A, %B %d %Y %H:%M:%S", &timeinfo);
  Serial.println(formattedTime);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi and get the current time
  connectWiFi();
  syncTime();
}

void loop() {
  // Print formatted date and time
  printDateTime();
  delay(1000);
}