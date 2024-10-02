#include <FirebaseESP32.h>
#include <WiFi.h>
#include <DHT.h>
#include <RCSwitch.h>

#define WIFI_SSID "Starlink Yehuin"
#define WIFI_PASSWORD "solom854"

#define DHTPIN 22
#define DHTTYPE DHT11
#define SEND_PIN 12

#define FIREBASE_HOST "esp32rinconada-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "77P1aUlOPjRGDOGVT6yzVpvOvJmRbFXcm1v5FzKB"

FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

DHT dht(DHTPIN, DHTTYPE);
RCSwitch mySwitch = RCSwitch();

const char* deviceID = "dFvwpoPyysZDF9ZVuBRfeKqpmGQ2";

unsigned long previousMillis = 0;
unsigned long previousReconnectMillis = 0;
const long interval = 6000; // Intervalo para enviar temperatura
const long reconnectInterval = 20000; // Intentar reconectar Firebase cada 30 segundos
bool firebaseConnected = true;

int rfProtocol = 1;
unsigned long rfCodeOn = 734728624; //CODIGO ENCENDIDO
unsigned long rfCodeOff = 734725384;  //CODIGO APAGADO

void setup() {
  Serial.begin(115200);
  dht.begin();
  
  mySwitch.enableTransmit(SEND_PIN);
  mySwitch.setProtocol(rfProtocol);
  mySwitch.setPulseLength(230);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Conectando a WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  Serial.println(" conectado.");

  firebaseConfig.host = FIREBASE_HOST;
  firebaseConfig.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&firebaseConfig, &firebaseAuth);
  Firebase.reconnectWiFi(true);
}

void reconnectWiFi() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconectando WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();
    unsigned long startAttemptTime = millis();

    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 10000) {
      delay(500);
      Serial.print(".");
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println(" reconectado.");
    } else {
      Serial.println("No se pudo reconectar al WiFi.");
    }
  }
}

void attemptReconnectFirebase() {
  if (millis() - previousReconnectMillis >= reconnectInterval) {
    previousReconnectMillis = millis();
    Firebase.begin(&firebaseConfig, &firebaseAuth);
    Serial.println("Intentando reconectar Firebase...");
    firebaseConnected = true;
  }
}

void loop() {
  reconnectWiFi(); // Verifica y reconecta WiFi si es necesario

  if (!firebaseConnected) {
    attemptReconnectFirebase();
  }

  if (Firebase.ready()) {
    String path = String("/users/") + String(deviceID) + "/LedStatus";
    if (Firebase.getString(firebaseData, path)) {
      String ledStatus = firebaseData.stringData();
      
      if (ledStatus == "1") {
        // Enciende el equipo (envía el código RF)
        mySwitch.send(rfCodeOn, 32);
        Serial.print("Código RF enviado para encender: ");
        Serial.println(rfCodeOn);

        // Después de enviar el código RF, desactiva ledStatus
        Firebase.setString(firebaseData, path, "0");
        Serial.println("LedStatus reseteado a 0");

        delay(100); // Retraso pequeño después de la acción

      } else if (ledStatus == "2") {
        // Apaga el equipo (envía el código RF)
        mySwitch.send(rfCodeOff, 32);
        Serial.print("Código RF enviado para apagar: ");
        Serial.println(rfCodeOff);

        // Después de enviar el código RF, desactiva ledStatus
        Firebase.setString(firebaseData, path, "0");
        Serial.println("LedStatus reseteado a 0");

        delay(100); // Retraso pequeño después de la acción
      }
    } else {
      Serial.print("Error al obtener dato: ");
      Serial.println(firebaseData.errorReason());
      firebaseConnected = false; // Si falla, marcamos Firebase como desconectado
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= interval) {
      previousMillis = currentMillis;

      float temperature = dht.readTemperature();
      if (isnan(temperature)) {
        Serial.println("Error al leer la temperatura");
      } else {
        Serial.print("Temperatura: ");
        Serial.println(temperature);

        // Guardar la temperatura en una clave fija, sobrescribiendo la anterior
        path = String("/users/") + String(deviceID) + "/TemperatureReadings/current";
        if (Firebase.setFloat(firebaseData, path, temperature)) {
          Serial.println("Temperatura enviada correctamente a Firebase");
        } else {
          Serial.print("Error al enviar temperatura: ");
          Serial.println(firebaseData.errorReason());
          firebaseConnected = false; // Si falla, marcamos Firebase como desconectado
        }
        delay(100);
      }
    }
  } else {
    Serial.println("Firebase no está listo. Intentando reconectar...");
    firebaseConnected = false; // Si Firebase no está listo, marcamos como desconectado
  }
}
