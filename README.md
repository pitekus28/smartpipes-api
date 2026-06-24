# Smart Pipes

## Descripción
Smart Pipes es un sistema de monitoreo de ductos que detecta posibles obstrucciones y presencia de lluvia en tiempo real.

## Integrantes
- Diego Fuentes
- Javier Fuentes
- Ignacio Tajmuch
- Felipe Madariaga

## Problema que aborda
El proyecto busca prevenir acumulación de agua, obstrucciones en ductos y situaciones que puedan generar rebalses o problemas en el entorno urbano.

## Funcionamiento general
El sistema utiliza un sensor ultrasónico para medir distancia dentro del ducto y un sensor de lluvia para detectar presencia de agua. Con esos datos, clasifica el estado del sistema en distintos niveles de alerta y muestra la información en una pantalla OLED.

## Componentes principales
-ESP32-S3 N16R8
-Sensor ultrasónico impermeable SR04M-2 / AJ-SR04M
-Sensor de humedad/lluvia MH-RD Raindrops Sensor de 2 pines
-Batería LiPo 3.7V 2000mAh
-Módulo Step-Up DC-DC 3.7V a 5V
-Resistencia 10kΩ
-Cables Dupont
-Protoboard / base de conexión interna/ Placa de expasion Esp 32 s3

## Estructura del repositorio
- `firmware/`: código del proyecto
- `hardware/`: esquema de conexión, componentes y montaje
- `diseno-3d/`: carcasa, modelos 3D y archivos de Fusion
- `testing/`: protocolo de pruebas, resultados y análisis de fallas
- `docs/`: presentación, informes y documentación general

## Cómo usar el proyecto
1. Abrir el archivo `.ino` en Arduino IDE
2. Instalar las librerías necesarias
3. Cargar el código en la placa
4. Visualizar resultados en la pantalla OLED y en el monitor serial

## Estado actual
Prototipo funcional en desarrollo dentro del curso Taller de Diseño.
