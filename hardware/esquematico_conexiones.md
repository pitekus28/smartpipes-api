<img width="2760" height="2520" alt="smartpipes_sensor1_UAI_esquematico_final" src="https://github.com/user-attachments/assets/5904500d-3e49-4aea-bac1-9eebd219587d" />
# Esquemático de conexiones SmartPipes

## ESP32-S3 N16R8

La ESP32-S3 es el controlador principal del sistema. Lee los sensores, calcula el porcentaje de llenado y envía los datos al dashboard web mediante WiFi.

## Alimentación

| Componente | Pin | Conexión |
|---|---|---|
| Batería LiPo 3.7V | + | IN+ Step-Up |
| Batería LiPo 3.7V | - | IN- Step-Up |
| Step-Up DC-DC | OUT+ 5V | 5V/VIN ESP32 |
| Step-Up DC-DC | OUT- GND | GND ESP32 |

## Sensor ultrasónico SR04M-2 / AJ-SR04M

| Pin sensor | Conexión |
|---|---|
| 5V | 5V/VIN ESP32 |
| GND | GND ESP32 |
| TX | GPIO18 ESP32 |
| RX | Sin conectar |

## Sensor humedad MH-RD Raindrops 2 pines

| Pin / elemento | Conexión |
|---|---|
| Pin 1 sensor humedad | 3V3 ESP32 |
| Pin 2 sensor humedad | GPIO4 ESP32 |
| Resistencia 10kΩ | Entre GPIO4 y GND |

## Nota importante

Todos los GND del sistema deben estar conectados en común:

- GND ESP32
- GND del sensor ultrasónico
- OUT- del Step-Up
- Negativo de batería
- GND de la resistencia pull-down

## Diagrama simplificado

```txt
Batería LiPo 3.7V
        |
        v
Step-Up DC-DC 3.7V a 5V
        |
        v
ESP32-S3 N16R8
        |
        |-- GPIO18 -> TX Sensor ultrasónico SR04M-2
        |-- 5V/VIN -> 5V Sensor ultrasónico
        |-- GND    -> GND Sensor ultrasónico
        |
        |-- GPIO4  -> Sensor humedad MH-RD
        |-- 3V3    -> Sensor humedad MH-RD
        |-- GPIO4  -> Resistencia 10kΩ -> GND
