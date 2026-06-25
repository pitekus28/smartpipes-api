# Protocolo, análisis de resultados y validación — SmartPipes

## 1. Objetivo de las pruebas

El objetivo de estas pruebas es validar que el prototipo SmartPipes funciona como un sistema IoT completo capaz de:

1. Medir la distancia física dentro de una alcantarilla o ducto simulado.
2. Calcular un porcentaje de llenado asociado al nivel de obstrucción.
3. Clasificar el estado del ducto mediante umbrales de riesgo.
4. Detectar presencia de humedad mediante el sensor MH-RD.
5. Enviar los datos por WiFi hacia una API desplegada en Railway.
6. Guardar las mediciones en una base de datos PostgreSQL en Aiven.
7. Visualizar los datos en un dashboard web con histórico y gráficos.

Estas pruebas buscan comprobar que el sistema no solo captura datos, sino que transforma esas mediciones en información útil para la toma de decisiones preventivas.

---

## 2. Condiciones de prueba

Las pruebas se realizaron con el prototipo SmartPipes Sensor 1 UAI compuesto por:

- ESP32-S3 N16R8.
- Sensor ultrasónico impermeable SR04M-2 / AJ-SR04M.
- Sensor de humedad MH-RD Raindrops de 2 pines.
- Resistencia pull-down de 10kΩ para el sensor de humedad.
- Batería LiPo 3.7V 2000mAh.
- Módulo Step-Up DC-DC 3.7V a 5V.
- API en Railway.
- Base de datos PostgreSQL en Aiven.
- Dashboard web SmartPipes.

La calibración usada para el prototipo fue:

| Condición física | Distancia medida | Porcentaje de llenado |
|---|---:|---:|
| Alcantarilla limpia o despejada | 100 cm o más | 0% |
| Alcantarilla casi tapada | 20 cm o menos | 100% |

El sistema calcula el porcentaje de llenado a partir de la distancia física medida por el sensor ultrasónico. A menor distancia, mayor es el porcentaje de llenado, porque significa que el agua, basura o sedimento se encuentra más cerca del sensor.

---

## 3. Procedimiento de prueba

### Prueba 1: Lectura del sensor ultrasónico

**Objetivo:** comprobar que el sensor ultrasónico mide distancia física de forma estable.

**Procedimiento:**

1. Encender la ESP32.
2. Verificar conexión WiFi desde el monitor serial.
3. Ubicar un objeto frente al sensor a distintas distancias.
4. Registrar las lecturas mostradas en el monitor serial.
5. Confirmar que la distancia medida cambia al acercar o alejar el objeto.
6. Verificar que el dato enviado a la API corresponde a la distancia medida.

**Criterio de éxito:**

La prueba se considera exitosa si el sensor entrega valores coherentes y variables según la distancia física del objeto, y si estos valores aparecen correctamente en el dashboard.

---

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

---

### Prueba 3: Clasificación de estado

**Objetivo:** verificar que el sistema clasifica correctamente el estado del ducto.

**Estados utilizados:**

| Porcentaje de llenado | Estado | Interpretación |
|---:|---|---|
| 0% - 19% | NORMAL | Ducto despejado |
| 20% - 39% | ATENCION | Acumulación leve |
| 40% - 59% | ALERTA | Posible bloqueo |
| 60% - 74% | RIESGO | Obstrucción alta |
| 75% - 89% | CRITICO | Casi colapsado |
| 90% - 100% | EMERGENCIA | Tapado total |

**Procedimiento:**

1. Probar distintas distancias frente al sensor.
2. Observar el porcentaje calculado.
3. Verificar que el estado mostrado corresponde al rango esperado.
4. Revisar que el mismo estado se guarde en la base de datos.
5. Confirmar que el dashboard muestra el último estado registrado.

**Criterio de éxito:**

La prueba se considera exitosa si el estado cambia de forma coherente con el porcentaje de llenado y permite diferenciar entre ducto normal, alerta, riesgo, crítico y emergencia.

---

### Prueba 4: Sensor de humedad

**Objetivo:** validar la lectura del sensor de humedad MH-RD.

**Procedimiento:**

1. Conectar el sensor MH-RD a 3V3 y GPIO4.
2. Conectar una resistencia de 10kΩ entre GPIO4 y GND.
3. Observar el valor ADC del sensor en seco.
4. Probar el sensor con presencia de agua o puenteando la señal para verificar la lectura.
5. Confirmar que el monitor serial cambia entre humedad detectada: SI / NO.

**Criterio de éxito:**

La prueba se considera exitosa si el sistema logra diferenciar entre ausencia y presencia de conducción eléctrica en el sensor. Durante las pruebas se comprobó que la resistencia pull-down evita falsos positivos cuando el pin queda flotante.

---

### Prueba 5: Envío de datos a la nube

**Objetivo:** comprobar que la ESP32 envía datos correctamente a la API en Railway.

**Procedimiento:**

1. Encender Aiven y verificar que la base de datos esté en estado Running.
2. Activar el deployment de Railway.
3. Conectar la ESP32 a WiFi.
4. Observar el monitor serial.
5. Verificar que aparezca `Codigo respuesta API: 200`.
6. Confirmar que la medición aparece en el dashboard.

**Criterio de éxito:**

La prueba se considera exitosa si cada medición enviada por la ESP32 recibe respuesta 200 desde la API y queda registrada en la base de datos.

---

### Prueba 6: Dashboard e histórico

**Objetivo:** verificar que el dashboard permite interpretar el estado actual e histórico del sistema.

**Procedimiento:**

1. Abrir el dashboard web.
2. Observar la tarjeta principal del Sensor 1 UAI.
3. Verificar distancia, porcentaje, humedad, batería y estado.
4. Revisar la tabla de últimas mediciones.
5. Revisar el histórico y gráficos del porcentaje de llenado.
6. Confirmar que al cambiar la distancia física, el dashboard actualiza los datos.

**Criterio de éxito:**

La prueba se considera exitosa si el dashboard muestra información actualizada, comprensible y útil para identificar tendencias o situaciones críticas.

---

## 4. Resultados obtenidos

Durante las pruebas, el sistema logró capturar datos reales de distancia y enviarlos correctamente a la nube. En el monitor serial se observaron respuestas exitosas de la API con código 200, lo que confirma que la comunicación entre la ESP32, Railway y Aiven funcionó correctamente.

Ejemplos de mediciones observadas:

| Distancia medida | Porcentaje de llenado | Humedad | Estado | Interpretación |
|---:|---:|---|---|---|
| 84.60cm | 19.30% | SI | ATENCION | 3:14;52 P.M. |
| 63.10cm | 46.10% | NO | ALERTA | 4:00;17 P.M. |
| 22.7cm | 96.6% | NO | EMERGENCIA | 4:19;01 P.M. |
| 40.9cm | 73.9% | NO | CRITICO | 4:58;38 P.M. |


La variable de humedad se interpreta como una señal complementaria. El estado principal del ducto se determina por la distancia y el porcentaje de llenado. La humedad permite identificar si existe presencia de agua o conducción en el sensor MH-RD, pero no reemplaza la medición ultrasónica. Por esta razón, una obstrucción seca podría marcar humedad NO, mientras que una condición con acumulación de agua debería marcar humedad SI.

Los resultados muestran que el sistema responde de manera lógica: cuando la distancia disminuye, el porcentaje de llenado aumenta y el estado pasa desde NORMAL hacia ATENCION, ALERTA, CRITICO o EMERGENCIA. La humedad funciona como una variable secundaria para complementar la interpretación, especialmente cuando el aumento de nivel está asociado a presencia de agua.

---

## 5. Análisis de resultados

### 5.1 Precisión de medición

El sensor ultrasónico respondió correctamente a cambios físicos de distancia. Cuando el objeto se acercaba al sensor, la distancia medida disminuía y el porcentaje de llenado aumentaba. Esto confirma que el principio de medición es adecuado para detectar acumulación o bloqueo dentro de un ducto.

Sin embargo, la precisión puede verse afectada por:

- Ángulo de instalación del sensor.
- Superficie irregular del agua o basura.
- Humedad directa sobre el cabezal.
- Interferencias físicas dentro del ducto.
- Rebotes del ultrasonido en superficies curvas.

Como mejora, se propone usar una carcasa que mantenga fijo el ángulo del sensor y aplicar un promedio de varias mediciones para reducir lecturas erráticas.

---

### 5.2 Latencia del sistema

La latencia observada fue baja para el objetivo del prototipo. Desde que la ESP32 captura una medición hasta que aparece en el dashboard, el tiempo depende de:

- Frecuencia de envío configurada en el firmware.
- Conexión WiFi.
- Respuesta de Railway.
- Escritura en base de datos Aiven.
- Actualización del dashboard.

Para pruebas se usó una frecuencia de envío corta, aproximadamente cada 10 segundos. Para una implementación real se recomienda aumentar el intervalo a 1, 3 o 5 minutos, ya que el fenómeno de obstrucción en una alcantarilla no requiere mediciones cada pocos segundos.

---

### 5.3 Tasa de error

Durante el desarrollo se identificaron tres fuentes principales de error:

1. **Sensor de humedad con falsos positivos:**  
   Inicialmente el sensor marcaba humedad incluso cuando estaba desconectado. La causa fue que GPIO4 quedaba flotando. Se corrigió agregando una resistencia pull-down de 10kΩ entre GPIO4 y GND.

2. **Dependencia de servicios en la nube:**  
   Si Aiven está apagado, Railway puede quedar activo pero no puede guardar ni leer mediciones. Esto significa que el sistema requiere que la base de datos esté encendida antes de activar la ESP32.

3. **Cambios en calibración física:**  
   Al medir la alcantarilla real se ajustó la referencia de ducto limpio a aproximadamente 100 cm. Esto obligó a recalibrar el cálculo del porcentaje de llenado y los estados del sistema.

---

### 5.4 Fallas encontradas y soluciones aplicadas

| Falla encontrada | Causa | Solución aplicada |
|---|---|---|
| Humedad siempre marcaba SI | GPIO4 flotante | Se agregó resistencia pull-down 10kΩ |
| Dashboard no cargaba después de apagar servicios | Railway o Aiven estaban detenidos | Se definió orden de encendido: Aiven → Railway → ESP32 |
| Mediciones antiguas afectaban pruebas nuevas | Base de datos con registros previos | Se limpió la tabla de mediciones para iniciar pruebas desde cero |
| Porcentaje no representaba bien la alcantarilla real | Calibración inicial de 150 cm no correspondía al ducto medido | Se ajustó la referencia limpia a 100 cm |

---

## 6. Validación contra el problema original

El problema original identificado por el proyecto SmartPipes corresponde a la falta de monitoreo preventivo de alcantarillados y desagües urbanos. En comunas como Santiago, Maipú o La Florida, la acumulación de basura, sedimento o agua puede generar obstrucciones que terminan en inundaciones, malos olores y problemas sanitarios. Actualmente, la mantención suele ser reactiva: se limpia o interviene cuando el problema ya ocurrió.

SmartPipes valida una solución preventiva porque permite detectar señales tempranas de obstrucción antes de que el sistema colapse. El prototipo mide distancia física dentro del ducto, calcula porcentaje de llenado y clasifica el nivel de riesgo. Estos datos permiten transformar una medición cruda en una decisión práctica.

Por ejemplo:

| Estado | Decisión sugerida |
|---|---|
| NORMAL | No requiere intervención |
| ATENCION | Monitorear evolución |
| ALERTA | Programar inspección preventiva |
| RIESGO | Priorizar revisión en terreno |
| CRITICO | Ejecutar limpieza urgente |
| EMERGENCIA | Intervenir inmediatamente |

La validación demuestra que el sistema cumple el ciclo IoT requerido:

**Dispositivo captura datos → genera información → permite tomar decisiones.**

La ESP32 captura datos reales mediante sensores, la API almacena los registros en una base de datos persistente, el dashboard visualiza el estado actual e histórico, y los estados de alerta permiten decidir cuándo inspeccionar o limpiar un punto crítico.

---

## 7. Conclusión de validación

Las pruebas realizadas muestran que SmartPipes cumple con el objetivo principal del proyecto: entregar información preventiva sobre el estado de un alcantarillado mediante un sistema IoT funcional.

El prototipo no soluciona físicamente una obstrucción, pero permite detectar condiciones de riesgo antes de que se produzca una inundación. Esto entrega valor a municipalidades o entidades responsables, ya que permite priorizar mantenciones, reducir respuestas reactivas y anticipar eventos críticos.

Como mejoras futuras se propone:

- Implementar medición real de porcentaje de batería mediante divisor de voltaje.
- Mejorar la impermeabilización del encapsulado.
- Instalar más sensores para validar el sistema en múltiples puntos.
- Agregar alertas automáticas por correo, WhatsApp o panel municipal.
- Incorporar geolocalización de sensores en un mapa.
