# Protocolo de Pruebas — SmartPipes

## 1. Objetivo de las pruebas

El objetivo de estas pruebas es validar que el prototipo SmartPipes funciona como un sistema IoT capaz de:

1. Medir la distancia física dentro de una alcantarilla o ducto simulado.
2. Calcular un porcentaje de llenado asociado al nivel de obstrucción.
3. Clasificar el estado del ducto mediante umbrales de riesgo.
4. Detectar presencia de humedad mediante el sensor MH-RD.
5. Enviar los datos por WiFi hacia una API desplegada en Railway.
6. Guardar las mediciones en una base de datos PostgreSQL en Aiven.
7. Visualizar los datos en un dashboard web con histórico y estados interpretables.

Estas pruebas buscan comprobar que el sistema no solo captura datos, sino que transforma esas mediciones en información útil para la toma de decisiones preventivas.

## 2. Condiciones de prueba

Las pruebas se realizaron con el prototipo SmartPipes Sensor 1 UAI compuesto por:

* ESP32-S3 N16R8
* Sensor ultrasónico impermeable SR04M-2 / AJ-SR04M
* Sensor de humedad MH-RD Raindrops de 2 pines
* Resistencia pull-down de 10kΩ para el sensor de humedad
* Batería LiPo 3.7V 2000mAh
* Módulo Step-Up DC-DC 3.7V a 5V
* API en Railway
* Base de datos PostgreSQL en Aiven
* Dashboard web SmartPipes

La calibración usada para el prototipo fue:

| Condición física                | Distancia medida | Porcentaje de llenado |
| ------------------------------- | ---------------: | --------------------: |
| Alcantarilla limpia o despejada |     100 cm o más |                    0% |
| Alcantarilla casi tapada        |    20 cm o menos |                  100% |

El sistema calcula el porcentaje de llenado a partir de la distancia física medida por el sensor ultrasónico. A menor distancia, mayor es el porcentaje de llenado, porque significa que el agua, basura o sedimento se encuentra más cerca del sensor.

## 3. Procedimiento de prueba

### Prueba 1: Lectura del sensor ultrasónico

**Objetivo:** comprobar que el sensor ultrasónico mide distancia física de forma estable.

**Procedimiento:**

1. Encender la ESP32.
2. Verificar conexión WiFi desde el monitor serial.
3. Ubicar un objeto frente al sensor a distintas distancias.
4. Registrar las lecturas mostradas en el dashboard.
5. Confirmar que la distancia medida cambia al acercar o alejar el objeto.
6. Verificar que el dato enviado a la API corresponde a la distancia medida.

**Criterio de éxito:**
La prueba se considera exitosa si el sensor entrega valores coherentes y variables según la distancia física del objeto, y si estos valores aparecen correctamente en el dashboard.

### Prueba 2: Cálculo de porcentaje de llenado

**Objetivo:** validar que la distancia medida se transforma correctamente en porcentaje de llenado.

**Procedimiento:**

1. Simular una condición despejada con una distancia cercana o superior a 100 cm.
2. Confirmar que el porcentaje de llenado se aproxima a 0%.
3. Simular una condición intermedia con una distancia entre 60 y 80 cm.
4. Confirmar que el porcentaje de llenado aumenta.
5. Simular una condición crítica acercando el objeto al sensor, bajo 30 cm.
6. Confirmar que el porcentaje de llenado se aproxima a valores altos.

**Criterio de éxito:**
La prueba se considera exitosa si el porcentaje aumenta cuando la distancia disminuye, representando correctamente el nivel de obstrucción.

### Prueba 3: Clasificación de estado

**Objetivo:** verificar que el sistema clasifica correctamente el estado del ducto.

**Estados utilizados:**

| Porcentaje de llenado | Estado     | Interpretación   |
| --------------------: | ---------- | ---------------- |
|              0% - 19% | NORMAL     | Ducto despejado  |
|             20% - 39% | ATENCION   | Acumulación leve |
|             40% - 59% | ALERTA     | Posible bloqueo  |
|             60% - 74% | RIESGO     | Obstrucción alta |
|             75% - 89% | CRITICO    | Casi colapsado   |
|            90% - 100% | EMERGENCIA | Tapado total     |

**Procedimiento:**

1. Probar distintas distancias frente al sensor.
2. Observar el porcentaje calculado.
3. Verificar que el estado mostrado corresponde al rango esperado.
4. Revisar que el mismo estado se guarde en la base de datos.
5. Confirmar que el dashboard muestra el último estado registrado.

**Criterio de éxito:**
La prueba se considera exitosa si el estado cambia de forma coherente con el porcentaje de llenado y permite diferenciar entre ducto normal, atención, alerta, riesgo, crítico y emergencia.

### Prueba 4: Sensor de humedad

**Objetivo:** validar la lectura del sensor de humedad MH-RD.

**Procedimiento:**

1. Conectar el sensor MH-RD a 3V3 y GPIO4.
2. Conectar una resistencia de 10kΩ entre GPIO4 y GND.
3. Observar el valor del sensor en seco.
4. Probar el sensor con presencia de agua o puenteando la señal para verificar la lectura.
5. Confirmar que el dashboard cambia entre humedad detectada: SI / NO.

**Criterio de éxito:**
La prueba se considera exitosa si el sistema logra diferenciar entre ausencia y presencia de conducción eléctrica en el sensor. Durante las pruebas se comprobó que la resistencia pull-down evita falsos positivos cuando el pin queda flotante.

### Prueba 5: Envío de datos a la nube

**Objetivo:** comprobar que la ESP32 envía datos correctamente a la API en Railway.

**Procedimiento:**

1. Verificar que Aiven esté en estado activo.
2. Verificar que Railway esté desplegado correctamente.
3. Conectar la ESP32 a WiFi.
4. Observar el dashboard.
5. Confirmar que cada nueva medición aparece registrada en el sistema.

**Criterio de éxito:**
La prueba se considera exitosa si las mediciones capturadas por la ESP32 quedan visibles en el dashboard y persisten en el sistema.

### Prueba 6: Dashboard e histórico

**Objetivo:** verificar que el dashboard permite interpretar el estado actual e histórico del sistema.

**Procedimiento:**

1. Abrir el dashboard web.
2. Observar la tarjeta principal del Sensor 1 UAI.
3. Verificar distancia, porcentaje, humedad, batería y estado.
4. Revisar la tabla de últimas mediciones.
5. Confirmar que al cambiar la distancia física, el dashboard actualiza los datos.

**Criterio de éxito:**
La prueba se considera exitosa si el dashboard muestra información actualizada, comprensible y útil para identificar estados de riesgo.

## 4. Evidencia asociada

La evidencia visual de estas pruebas se encuentra en la carpeta `testing/evidencia/`, donde se incluyen capturas del dashboard correspondientes a distintos estados observados del sistema:

* ATENCION
* ALERTA
* CRITICO
* EMERGENCIA
