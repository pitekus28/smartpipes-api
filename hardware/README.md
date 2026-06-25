# Hardware — SmartPipes

## Descripción

Esta carpeta contiene la documentación del hardware del proyecto SmartPipes. Aquí se incluyen los componentes utilizados en el prototipo, el esquemático de conexiones y la información necesaria para comprender cómo fue implementado físicamente el sistema.

El hardware fue diseñado para capturar datos reales del entorno mediante sensores, procesarlos con una ESP32-S3 y alimentar el sistema de forma portátil, manteniendo una arquitectura coherente con el problema abordado: el monitoreo preventivo de ductos y alcantarillados.

---

## Contenido de la carpeta

### 1. BOM (Bill of Materials)

La lista de materiales resume los componentes utilizados para construir el prototipo, junto con su cantidad, especificación, función y costo aproximado.

Incluye elementos principales como:

* ESP32-S3 N16R8
* sensor ultrasónico impermeable AJ-SR04M / SR04M
* sensor de humedad MH-RD
* batería LiPo
* módulo Step-Up DC-DC
* cables, resistencia pull-down, tornillos y materiales de fabricación

La BOM permite estimar el costo total del prototipo y documentar la viabilidad de replicación del sistema.

### 2. Esquemático de conexiones

El esquemático documenta la conexión entre la ESP32-S3, los sensores y el sistema de alimentación.

Se detallan:

* alimentación desde batería LiPo hacia el módulo Step-Up
* entrega de 5V a la ESP32 y al sensor ultrasónico
* lectura del sensor ultrasónico mediante GPIO18
* lectura del sensor de humedad mediante GPIO4
* uso de resistencia pull-down de 10kΩ para evitar falsos positivos en el sensor de humedad
* conexión común de tierras (GND) en todo el sistema

---

## Arquitectura de hardware

El sistema físico se compone de tres subsistemas principales:

### 1. Control y procesamiento

La **ESP32-S3 N16R8** actúa como controlador principal. Se encarga de:

* leer los sensores,
* calcular el porcentaje de llenado,
* clasificar el estado del ducto,
* y enviar los datos al sistema web mediante WiFi.

### 2. Sensado

El prototipo utiliza dos sensores:

* **AJ-SR04M / SR04M-2**: mide la distancia física al fondo del ducto o a una obstrucción.
* **MH-RD Raindrops**: detecta humedad o presencia de agua como variable complementaria.

La elección del sensor ultrasónico impermeable fue especialmente importante para hacer coherente el sistema con un contexto húmedo o expuesto al agua.

### 3. Alimentación

La energía del sistema proviene de una **batería LiPo de 3.7V**, cuyo voltaje es elevado mediante un **módulo Step-Up DC-DC** para alimentar correctamente la ESP32 y el sensor ultrasónico.

---

## Consideraciones técnicas importantes

### Sensor de humedad

El sensor MH-RD requiere una **resistencia pull-down de 10kΩ** entre GPIO4 y GND. Esta resistencia evita que el pin quede flotando, lo que podría generar lecturas falsas de humedad.

### Sensor ultrasónico

El sensor AJ-SR04M fue elegido por su mayor pertinencia para un sistema expuesto a agua o humedad, en comparación con sensores ultrasónicos más básicos como el HC-SR04.

### GND común

Todos los negativos del sistema deben compartir tierra común:

* GND de la ESP32
* GND del sensor ultrasónico
* negativo de batería
* salida negativa del Step-Up
* GND de la resistencia pull-down

Esto es fundamental para asegurar lecturas estables y un funcionamiento correcto del sistema.

---

## Justificación general del hardware

Los componentes seleccionados permiten construir un prototipo funcional, portátil y técnicamente coherente con el problema del proyecto. El sistema combina:

* procesamiento con conectividad WiFi,
* medición física de nivel u obstrucción,
* detección complementaria de humedad,
* y alimentación autónoma mediante batería.

Además, el montaje fue pensado para integrarse dentro de un encapsulado fabricado en 3D, con fijaciones desmontables y acceso a componentes internos para reparación o ajuste.

---

## Archivos esperados en esta carpeta

* `BOM.md`, `BOM.xlsx` o equivalente
* `esquematico.pdf` o imagen del esquemático
* fotografías del prototipo ensamblado
* documentación complementaria de conexiones si aplica

---

## Observaciones

La documentación contenida en esta carpeta permite entender, replicar y justificar la implementación física del prototipo SmartPipes dentro del contexto del curso TEI201.

