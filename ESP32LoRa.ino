//Este codigo es para el ESP32 con wifi que se comunica a traves de RadioFrecuencia LoRa con el Arduino que no tiene conexion a internet.
//La distancia depende del campo y de la antena. Ver tutoriales para crear una antena de radiofrecuencia 433mhz robusta.
#include <FirebaseESP32.h>
#include <WiFi.h>
#include <SPI.h>
#include <LoRa.h>
#include <esp_task_wdt.h>

// Configuración de la red WiFi
#define WIFI_SSID "SSID WiFi"
#define WIFI_PASSWORD "Contra WiFi"

// Configuración de los pines de LoRa
const int csPin = 5;      // LoRa radio chip select
const int resetPin = 14;  // LoRa radio reset
const int irqPin = 4;     // Must be a hardware interrupt pin

// Configuración de Firebase
#define FIREBASE_HOST "Consultar Firebase_Host en Firebase"
#define FIREBASE_AUTH "ID mail del cliente"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

// Identificador único del dispositivo
const char* deviceID = "ID mail Cliente"; // Cambia esto al identificador de tu dispositivo
unsigned long lastSendTime = 0;
byte msgCount = 0;
byte statusBefore = 0;
void setup() {
  Serial.begin(115200);

  // Inicializar LoRa
  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(433E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }

  esp_task_wdt_init(10, true);
  esp_task_wdt_add(NULL);

  // Conexión a WiFi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" conectado.");

  // Configuración de Firebase
  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;  // Token de autenticación
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Resetear el Watchdog Timer en cada ciclo del loop
  esp_task_wdt_reset();

  // Reconexión a WiFi si es necesario
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconectando WiFi...");
    WiFi.reconnect();
  }

  // Verificar si Firebase está listo
  if (Firebase.ready()) {
    

    // Recibir paquete de LoRa (temperatura desde Arduino)
    int packetSize = LoRa.parsePacket();
    if (packetSize) {
      String temperatureStr = "";
      while (LoRa.available()) {
        temperatureStr += (char)LoRa.read();
      }
      float temperature = temperatureStr.toFloat();
      Serial.print("Temperatura recibida: ");
      Serial.println(temperature);
      Serial.println(temperatureStr);

      // Enviar la temperatura a Firebase
      String path = String("/users/") + String(deviceID) + "/TemperatureReadings/" + String(millis());
      if (!Firebase.setFloat(firebaseData, path, temperature)) {
        Serial.print("Error al enviar temperatura: ");
        Serial.println(firebaseData.errorReason());
      }
    }
  } else {
    Serial.println("Firebase no está listo, intentando de nuevo...");
    delay(1000); // Retraso para evitar saturación de peticiones
  }
  unsigned long currentMillis = millis();
  if (currentMillis - lastSendTime > 2000) {
    lastSendTime = currentMillis;
    String path = String("/users/") + String(deviceID) + "/LedStatus";
    if (Firebase.getString(firebaseData, path)) {
      String ledStatus = firebaseData.stringData();
      if (ledStatus == "1" && statusBefore == 0) {
        LoRa.beginPacket();
        LoRa.print("CALENTADOR ON");
        LoRa.endPacket();
        Serial.println("Sent command: CALENTADOR ON");
        statusBefore = 1;
      } else if(ledStatus == "0" && statusBefore == 1){
        LoRa.beginPacket();
        LoRa.print("CALENTADOR OFF");
        LoRa.endPacket();
        Serial.println("Sent command: CALENTADOR OFF");
        statusBefore = 0;
      }
    } else {
        Serial.print("Error al obtener dato: ");
        Serial.println(firebaseData.errorReason());
    }
  }
}
