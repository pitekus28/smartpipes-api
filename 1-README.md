# SmartPipes

SmartPipes es un prototipo IoT para el monitoreo preventivo de alcantarillados. El sistema mide la distancia física entre el sensor y el nivel de agua, sedimento u obstrucción dentro de una alcantarilla, calcula un porcentaje de llenado, detecta humedad y envía los datos a una base de datos en la nube para visualizarlos en un dashboard web.

## Integrantes

* Javier Fuentes
* Diego Fuentes
* Ignacio Tajmuch
* Felipe Madariaga

## Problema que resuelve

Las obstrucciones en alcantarillados pueden generar rebalses, malos olores, inundaciones y problemas sanitarios. SmartPipes busca detectar acumulación o riesgo de obstrucción de forma temprana mediante sensores instalados en el interior de una alcantarilla.

## Arquitectura del sistema

El flujo del sistema es:

ESP32-S3 → Railway API → Aiven PostgreSQL → Dashboard Web

La ESP32 captura datos del entorno, calcula indicadores y los envía por WiFi a una API desplegada en Railway. La API almacena los datos en una base de datos PostgreSQL en Aiven. El dashboard web permite visualizar el estado actual del sensor y el histórico de mediciones.

## Componentes principales

* ESP32-S3 N16R8
* Sensor ultrasónico impermeable SR04M-2 / AJ-SR04M
* Sensor de humedad/lluvia MH-RD Raindrops Sensor de 2 pines
* Batería LiPo 3.7V 2000mAh
* Módulo Step-Up DC-DC 3.7V a 5V
* Resistencia 10kΩ para pull-down del sensor de humedad
* Cables Dupont
* Protoboard o base de conexión interna

## Sensores y pines usados

### Sensor ultrasónico SR04M-2 / AJ-SR04M

* 5V → 5V/VIN ESP32
* GND → GND ESP32
* TX → GPIO18 ESP32
* RX → Sin conectar

### Sensor humedad MH-RD 2 pines

* Un pin del sensor → 3V3 ESP32
* Otro pin del sensor → GPIO4 ESP32
* GPIO4 → resistencia 10kΩ → GND

## Calibración usada

La alcantarilla se calibró considerando:

* 100 cm o más: alcantarilla limpia, 0% de llenado.
* 20 cm o menos: alcantarilla casi tapada, 100% de llenado.

El porcentaje de llenado se calcula con la distancia física medida por el sensor ultrasónico.

## Estados del sistema

* NORMAL: 0% a 19%
* ATENCION: 20% a 39%
* ALERTA: 40% a 59%
* RIESGO: 60% a 74%
* CRITICO: 75% a 89%
* EMERGENCIA: 90% a 100%

## Dashboard

Dashboard web:

https://smartpipes-api-production.up.railway.app

El dashboard permite visualizar:

* Estado actual del sensor
* Distancia física medida
* Porcentaje de llenado
* Humedad detectada
* Batería
* Histórico mensual
* Gráficas de porcentaje de llenado

## Instrucciones de uso

1. Encender la base de datos Aiven.
2. Activar el deployment en Railway.
3. Conectar la ESP32 a energía.
4. Verificar en el monitor serial que aparezca `Codigo respuesta API: 200`.
5. Abrir el dashboard web.
6. Observar las mediciones en tiempo real.

## Estructura del repositorio

* `firmware/`: código de la ESP32.
* `hardware/`: esquemático, BOM y fotografías del montaje.
* `diseño-3d/`: archivos de Fusion 360, STL, planos y renders.
* `testing/`: protocolo de pruebas, resultados, logs y capturas.
* `docs/`: documentación del proyecto.
* `public/`: dashboard web.
* `server.js`: API backend.
