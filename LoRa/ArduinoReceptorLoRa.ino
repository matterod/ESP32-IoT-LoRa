//Este codigo es para un Arduino sin conexion a internet, conectado a un modulo SX1278.

#include <SPI.h>
#include <LoRa.h>
#include <DHT.h>
#include <RCSwitch.h>

// Definir los pines usados por el módulo LoRa
const int csPin = 10;     // Selección de chip de la radio LoRa
const int resetPin = 9;   // Reset de la radio LoRa
const int irqPin = 2;     // Debe ser un pin de interrupción por hardware

// Definir el pin y tipo del sensor DHT11
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// Pines y variables de Radiofrecuencia (RF)
#define SEND_PIN 12
RCSwitch mySwitch = RCSwitch();
int rfProtocol = 1;
unsigned long rfCodeOn = /* El código se tiene que obtener de un receptor RF para encender */; // Código RF de encendido
unsigned long rfCodeOff = /* El código se tiene que obtener de un receptor RF para apagar */;  // Código RF de apagado

// Variables para el control de tiempos
unsigned long lastTemperatureSendTime = 0;
unsigned long lastCommandReceiveTime = 0;

void setup() {
  Serial.begin(9600);

  // Configurar el módulo LoRa
  LoRa.setPins(csPin, resetPin, irqPin);
  
  Serial.println("Receptor LoRa");

  // Iniciar el módulo LoRa en la frecuencia local
  if (!LoRa.begin(433E6)) {
    Serial.println("¡Error al iniciar LoRa!");
    while (1);
  }

  // Inicializar el sensor DHT
  dht.begin();

  // Inicializar RF (Radiofrecuencia)
  mySwitch.enableTransmit(SEND_PIN);
  mySwitch.setProtocol(rfProtocol);
  mySwitch.setPulseLength(230);
}

void loop() {
  // Comprobar si hay paquetes LoRa entrantes
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    String contents = "";
    while (LoRa.available()) {
      contents += (char)LoRa.read();
    }

    Serial.print("Paquete recibido: ");
    Serial.println(contents);

    // Comprobar el comando y enviar código RF
    if (contents == "Calentador ON") {
      Serial.println("Comando recibido: LED ON");
      // Enviar el código RF de encendido
      mySwitch.send(rfCodeOn, 32); // Enviar código RF de encendido
      Serial.print("Código RF enviado para encender: ");
      Serial.println(rfCodeOn);
      delay(300);

    } else if (contents == "Calentador OFF") {
      Serial.println("Comando recibido: LED OFF");
      // Enviar el código RF de apagado
      mySwitch.send(rfCodeOff, 32); // Enviar código RF de apagado
      Serial.print("Código RF enviado para apagar: ");
      Serial.println(rfCodeOff);
      delay(300);
    }

    lastCommandReceiveTime = millis(); // Actualiza el tiempo del último comando recibido
  }

  // Enviar la temperatura cada 5 segundos
  unsigned long currentTime = millis();

  if (currentTime - lastTemperatureSendTime > 600000) {
    float temperature = dht.readTemperature();

    if (!isnan(temperature)) {
      Serial.print("Enviando temperatura: ");
      Serial.println(temperature);

      LoRa.beginPacket();
      LoRa.print(temperature);
      LoRa.endPacket();
    }

    lastTemperatureSendTime = currentTime; // Actualiza el tiempo del último envío de temperatura
  }

  // Otras tareas o operaciones aquí
}
