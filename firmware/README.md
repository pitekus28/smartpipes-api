# Firmware — SmartPipes

## Descripción

Esta carpeta contiene el firmware principal del sistema SmartPipes. El programa está desarrollado para una **ESP32-S3** y se encarga de capturar datos del entorno, procesarlos y enviarlos a la nube para su visualización en un dashboard web.

El firmware mide la distancia interna de un ducto mediante un sensor ultrasónico impermeable, detecta presencia de humedad, calcula el porcentaje de llenado y clasifica el estado del sistema según niveles de riesgo.

---

## Funciones principales del firmware

El código implementa las siguientes tareas:

* conexión de la ESP32 a una red WiFi
* lectura del sensor ultrasónico AJ-SR04M / SR04M-2
* filtrado básico mediante promedio de múltiples lecturas
* lectura del sensor de humedad MH-RD
* cálculo del porcentaje de llenado del ducto
* clasificación del estado del sistema
* impresión de resultados en el monitor serial
* envío de datos en formato JSON a una API desplegada en Railway

---

## Variables que genera el sistema

El firmware obtiene y procesa las siguientes variables:

* **distancia_cm**: distancia medida entre el sensor y el fondo u obstrucción
* **porcentaje_lleno**: estimación del nivel de llenado del ducto
* **humedad**: presencia o ausencia de humedad detectada
* **estado**: clasificación del ducto según el nivel de riesgo
* **bateria_voltaje**: valor de batería enviado como referencia

---

## Calibración utilizada

La lógica del sistema utiliza dos referencias físicas:

* **100 cm o más** → ducto limpio → **0% lleno**
* **20 cm o menos** → ducto casi tapado → **100% lleno**

Con esto, el firmware transforma una distancia física en una estimación de llenado útil para interpretar el estado del ducto.

---

## Estados definidos

El sistema clasifica el ducto en los siguientes niveles:

| Estado     | Interpretación   |
| ---------- | ---------------- |
| NORMAL     | Ducto despejado  |
| ATENCION   | Acumulación leve |
| ALERTA     | Posible bloqueo  |
| RIESGO     | Obstrucción alta |
| CRITICO    | Casi colapsado   |
| EMERGENCIA | Tapado total     |
| ERROR      | Falla de sensor  |

---

## Hardware esperado por el firmware

### Microcontrolador

* ESP32-S3 N16R8

### Sensor ultrasónico

* AJ-SR04M / SR04M-2

### Sensor de humedad

* MH-RD Raindrops de 2 pines

### Otros elementos

* resistencia pull-down de 10kΩ
* batería LiPo 3.7V
* módulo Step-Up DC-DC 3.7V a 5V

---

## Pines utilizados

### Sensor ultrasónico

* `GPIO18` → señal del sensor ultrasónico

### Sensor de humedad

* `GPIO4` → lectura del sensor MH-RD

---

## Flujo de funcionamiento

1. La ESP32 se conecta a WiFi.
2. El sensor ultrasónico mide la distancia.
3. El sistema calcula el porcentaje de llenado.
4. El sensor de humedad detecta presencia de agua o conducción.
5. Se evalúa el estado del ducto.
6. Los datos se imprimen en el monitor serial.
7. Se envían a la API en Railway.
8. El ciclo se repite aproximadamente cada 10 segundos.

---

## Servicios externos utilizados

El firmware está preparado para trabajar con:

* una **API en Railway**
* una **base de datos PostgreSQL en Aiven**
* un **dashboard web SmartPipes**

---

## Cómo cargar el firmware

1. Abrir el archivo `.ino` en Arduino IDE.
2. Seleccionar la placa ESP32 correspondiente.
3. Revisar las credenciales de WiFi y la URL de la API.
4. Verificar la conexión de sensores y alimentación.
5. Cargar el código en la ESP32.
6. Abrir el monitor serial a **115200 baudios** para revisar el funcionamiento.

---

## Archivos esperados en esta carpeta

* `smart_pipes.ino` o archivo principal equivalente
* este `README.md`

---

## Observaciones

El firmware fue adaptado para el uso de un sensor ultrasónico impermeable, más coherente con el contexto del proyecto, orientado al monitoreo de ductos y alcantarillas con posible presencia de humedad o agua.
