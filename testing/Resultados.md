# Resultados y Validación — SmartPipes

## 1. Resultados obtenidos

Durante las pruebas, el sistema logró capturar datos reales de distancia y visualizarlos correctamente en el dashboard. Las mediciones registradas muestran que el sistema cambia de estado de forma coherente según la distancia física detectada y el porcentaje de llenado calculado.

Los siguientes registros corresponden a datos reales medidos durante la validación del prototipo:

| Fecha y hora              | Sensor       | Distancia (cm) | Porcentaje de llenado | Humedad | Estado     |
| ------------------------- | ------------ | -------------: | --------------------: | ------- | ---------- |
| 24-06-2026, 3:14:52 p. m. | Sensor 1 UAI |          84.60 |                19.30% | SI      | ATENCION   |
| 24-06-2026, 4:00:17 p. m. | Sensor 1 UAI |          63.10 |                46.10% | NO      | ALERTA     |
| 24-06-2026, 4:19:01 p. m. | Sensor 1 UAI |          22.70 |                96.60% | NO      | EMERGENCIA |
| 24-06-2026, 4:56:38 p. m. | Sensor 1 UAI |          40.90 |                73.90% | NO      | CRITICO    |

## 2. Interpretación de resultados

Los datos muestran que el sistema responde de manera lógica frente a distintos niveles de obstrucción:

* Con **84.60 cm**, el sistema mostró **ATENCION**, indicando una condición de acumulación leve.
* Con **63.10 cm**, el sistema cambió a **ALERTA**, mostrando un nivel intermedio de riesgo.
* Con **40.90 cm**, el sistema mostró **CRITICO**, reflejando una condición cercana al colapso.
* Con **22.70 cm**, el sistema alcanzó el estado **EMERGENCIA**, correspondiente a una condición de obstrucción severa.

Estos resultados demuestran que, a medida que la distancia libre disminuye, el porcentaje de llenado aumenta y el sistema eleva el nivel de riesgo.

La humedad se interpreta como una variable complementaria. El estado principal del ducto se determina por la distancia y el porcentaje de llenado, mientras que la humedad ayuda a contextualizar si existe presencia de agua o conducción en el sensor MH-RD.

## 3. Comportamiento observado del sistema

El sensor ultrasónico respondió correctamente a cambios físicos de distancia. Al acercar un objeto al sensor, la distancia medida disminuyó y el porcentaje de llenado aumentó. Esto confirma que el principio de medición es adecuado para representar acumulación o bloqueo dentro de un ducto.

El tiempo de respuesta observado fue adecuado para los fines del prototipo. La información capturada por la ESP32 fue visible en el dashboard en una escala de tiempo útil para monitoreo preventivo.

Durante el desarrollo también se identificaron fuentes de error relevantes:

* posibles falsos positivos del sensor de humedad cuando el pin quedaba flotante,
* dependencia del orden de encendido de servicios cloud,
* y necesidad de recalibrar la referencia de ducto limpio según las dimensiones reales del caso.

## 4. Falla principal encontrada y solución aplicada

La principal dificultad del desarrollo fue el cambio del sensor ultrasónico originalmente utilizado.

Inicialmente el sistema fue adaptado para trabajar con el sensor **HC-SR04**. Sin embargo, dado que el proyecto está orientado a monitoreo de alcantarillas y ductos con posible presencia de humedad o agua, ese sensor no era técnicamente la mejor alternativa para una aplicación realista.

Por esta razón se decidió reemplazarlo por el sensor impermeable **AJ-SR04M**. Este cambio implicó:

* modificar parte del código ya adaptado,
* reajustar la lógica de lectura,
* recalibrar el sistema,
* y volver a integrar el prototipo completo.

Además, durante el desarrollo se detectaron y corrigieron otras fallas menores:

| Falla encontrada                                                      | Causa                                                     | Solución aplicada                                      |
| --------------------------------------------------------------------- | --------------------------------------------------------- | ------------------------------------------------------ |
| Humedad siempre marcaba SI                                            | GPIO4 flotante                                            | Se agregó resistencia pull-down de 10kΩ                |
| Dashboard no cargaba correctamente si los servicios estaban detenidos | Dependencia de Railway y Aiven                            | Se definió orden de encendido: Aiven → Railway → ESP32 |
| Registros anteriores afectaban pruebas nuevas                         | Base de datos con mediciones acumuladas                   | Se limpiaron mediciones previas antes de validar       |
| Porcentaje no representaba bien el ducto real                         | Calibración inicial distinta a la condición física medida | Se ajustó la referencia limpia a 100 cm                |

## 5. Validación contra el problema original

El problema original identificado por SmartPipes corresponde a la falta de monitoreo preventivo de alcantarillados y desagües urbanos. En muchos casos, la mantención ocurre de forma reactiva, cuando la obstrucción o acumulación ya generó inundación, malos olores o problemas sanitarios.

SmartPipes valida una solución preventiva porque permite detectar señales tempranas de obstrucción antes de que el sistema colapse. El prototipo mide distancia física dentro del ducto, calcula porcentaje de llenado y clasifica el nivel de riesgo. Estos datos transforman una medición cruda en una decisión práctica.

Por ejemplo:

| Estado     | Decisión sugerida               |
| ---------- | ------------------------------- |
| NORMAL     | No requiere intervención        |
| ATENCION   | Monitorear evolución            |
| ALERTA     | Programar inspección preventiva |
| RIESGO     | Priorizar revisión en terreno   |
| CRITICO    | Ejecutar limpieza urgente       |
| EMERGENCIA | Intervenir inmediatamente       |

De esta forma, el sistema cumple el ciclo IoT requerido por el proyecto:

**Dispositivo captura datos → genera información → permite tomar decisiones**

La ESP32 captura datos reales mediante sensores, el sistema los almacena, el dashboard visualiza el estado actual e histórico, y los niveles de alerta permiten decidir cuándo monitorear, inspeccionar o intervenir un punto crítico.

## 6. Conclusión

Las pruebas realizadas muestran que SmartPipes cumple con el objetivo principal del proyecto: entregar información preventiva sobre el estado de un alcantarillado mediante un sistema IoT funcional.

El prototipo no resuelve físicamente una obstrucción, pero sí permite detectar condiciones de riesgo antes de que se produzca una falla mayor. Esto entrega valor para una gestión más preventiva y menos reactiva del problema.

Como mejoras futuras se propone:

* implementar medición real de porcentaje de batería mediante divisor de voltaje,
* mejorar la impermeabilización del encapsulado,
* instalar más sensores para validar el sistema en múltiples puntos,
* agregar alertas automáticas por correo, WhatsApp o panel municipal,
* incorporar geolocalización de sensores en un mapa.
