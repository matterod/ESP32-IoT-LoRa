# Control de Dispositivo con ESP32 mediante LoRa

Este repositorio contiene el código para un sistema de control remoto basado en **ESP32** y **comunicación LoRa**. El proyecto permite la operación de dispositivos en áreas rurales sin acceso a Internet, como el control de la calefacción de una cabaña en **Tierra del Fuego**, donde las temperaturas invernales suelen ser muy bajas.

## Descripción del Proyecto

El sistema se basa en dos microcontroladores **ESP32**:
1. Un ESP32 conectado a Internet con una **antena LoRa**.
2. Otro ESP32 en una zona rural, sin acceso a Internet, que recibe comandos a través de LoRa y controla dispositivos como la calefacción.

Este sistema ofrece la posibilidad de controlar la temperatura de la cabaña de forma remota, permitiendo que los usuarios puedan llegar a un espacio caliente, incluso en inviernos severos.

## Funcionalidades

- **Control remoto de dispositivos**: El proyecto está diseñado para controlar la calefacción, pero puede ampliarse para manejar diferentes dispositivos o sensores.
- **Comunicación LoRa**: Utiliza la tecnología LoRa para transmitir comandos entre los ESP32.
- **Acceso personalizado**: Solo el propietario del dispositivo, identificado por su correo electrónico, tiene acceso a la información y el control del ESP32.
- **Expansión a otros sensores y actuadores**: El proyecto puede ampliarse para incluir sensores de temperatura, humedad, entre otros.

## Proceso de Fabricación

### 1. Creación de la PCB

Para este proyecto, se fabricó una **PCB personalizada** utilizando el método de **ácido férrico y planchado**. Este proceso incluye los siguientes pasos:

1. **Diseño del circuito**: Se diseñó el circuito en un software de diseño de PCB.
2. **Impresión y planchado**: El diseño se imprimió en papel especial y luego se transfirió a una placa de cobre mediante calor (planchado).
3. **Corrosión con ácido férrico**: La placa de cobre fue sumergida en ácido férrico para corroer las áreas no cubiertas por el diseño, dejando únicamente las trazas de cobre.
4. **Taladrado y ensamblaje**: Se taladraron los agujeros para los componentes, y luego se soldaron los componentes electrónicos.

### 2. Ensamblaje del Dispositivo

Una vez que la PCB estuvo lista, se ensambló con los siguientes componentes:
- **ESP32**: El cerebro del dispositivo que gestiona la conectividad WiFi y LoRa.
- **Módulo LoRa SX1278**: Permite la comunicación de largo alcance entre los ESP32.
- **Modulo RF FS1000A**: Utilizado para encender y apagar la calefacción.
- **Sensores (opcional)**: Como el **DHT11** para medir la temperatura y ajustar la calefacción.

### 3. Diseño y Fabricación de la Carcasa

El dispositivo cuenta con una **carcasa personalizada**, diseñada a medida utilizando un programa de diseño 3D y fabricada mediante **impresión 3D**. La carcasa fue creada para proteger los componentes electrónicos y dar un acabado estético y funcional al dispositivo.

## Imágenes del Proyecto

Imágenes del dispositivo final y carcasa.

<img src="images/Circuito.jpeg" alt="Vista del dispositivo final" width="400"/>
<img src="images/Carcasa.jpeg" alt="Carcasa impresa en 3D" width="400"/>

## Cómo Usar Este Repositorio

1. **Cloná este repositorio** en tu compu.
2. **Cargá el código en tu ESP32** usando el **Arduino IDE** (fijate en los archivos del repo para más info).
3. **Configurá los dispositivos** con los módulos LoRa y hacé las conexiones eléctricas que hagan falta.
4. **Probá la comunicación LoRa** entre los ESP32 y chequeá que funcione bien el control remoto del equipo.

## Contribuciones

Si te pinta mejorar este proyecto, sos más que bienvenido a mandar un **pull request** o abrir un **issue** para reportar errores o tirar alguna idea.

---

Desarrollado por **Mateo Rodriguez y Agustin Trachta**.
