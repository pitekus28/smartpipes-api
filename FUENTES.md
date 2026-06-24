# FUENTES.md

Este documento declara las librerías, plataformas, herramientas externas y uso de inteligencia artificial utilizados en el desarrollo del prototipo SmartPipes.

## Librerías utilizadas

| Librería | Uso en el proyecto | Fuente |
|---|---|---|
| WiFi.h | Conexión WiFi de la ESP32 | Librería incluida en el core de ESP32 para Arduino |
| HTTPClient.h | Envío de datos HTTP/HTTPS desde ESP32 hacia la API | Librería incluida en el core de ESP32 para Arduino |
| WiFiClientSecure.h | Conexión HTTPS segura desde la ESP32 hacia Railway | Librería incluida en el core de ESP32 para Arduino |
| express | Servidor backend en Node.js para recibir datos y servir dashboard | https://www.npmjs.com/package/express |
| pg | Conexión entre Node.js y PostgreSQL | https://www.npmjs.com/package/pg |
| cors | Permite solicitudes entre cliente y servidor | https://www.npmjs.com/package/cors |
| dotenv | Manejo de variables de entorno | https://www.npmjs.com/package/dotenv |
| Chart.js | Visualización de gráficos históricos en el dashboard | https://www.chartjs.org/ |

## Plataformas utilizadas

| Plataforma | Uso en el proyecto |
|---|---|
| Arduino IDE | Desarrollo y carga del firmware en la ESP32 |
| GitHub | Repositorio del proyecto |
| Railway | Despliegue de la API y dashboard web |
| Aiven PostgreSQL | Base de datos persistente para almacenar mediciones |
| Fusion 360 | Diseño 3D de carcasa y gemelo digital |
| Fritzing / herramienta equivalente | Desarrollo del esquemático de conexión |

## Código externo adaptado

No se copió código externo completo desde repositorios de terceros. Se utilizaron ejemplos generales de conexión WiFi, solicitudes HTTP y manejo de servidores Express como referencia conceptual, adaptándolos al caso específico de SmartPipes.

## Uso de Inteligencia Artificial

### Desarrollo del firmware ESP32

- Herramienta: ChatGPT
- Uso: Apoyo para estructurar y comentar el firmware de la ESP32.
- Adaptación: El código fue ajustado al hardware real utilizado: ESP32-S3 N16R8, sensor ultrasónico SR04M-2 usando TX como lectura hacia GPIO18, sensor de humedad MH-RD en GPIO4 y envío de datos hacia Railway.
- Comprensión: El equipo entiende que el firmware mide distancia, calcula porcentaje de llenado, evalúa un estado mediante umbrales, detecta humedad y envía un JSON por HTTPS a la API.

### Desarrollo de API y dashboard

- Herramienta: ChatGPT
- Uso: Apoyo en la creación de la API con Node.js, Express y PostgreSQL, además del dashboard web.
- Adaptación: El backend fue configurado con la base de datos Aiven PostgreSQL del proyecto y desplegado en Railway. El dashboard fue adaptado para mostrar Sensor 1 UAI, distancia física, porcentaje de llenado, humedad, batería, estado e histórico mensual.
- Comprensión: El equipo entiende que Railway recibe las mediciones desde la ESP32, las guarda en Aiven y luego entrega los datos al dashboard mediante rutas API.

### Documentación y organización de GitHub

- Herramienta: ChatGPT
- Uso: Apoyo en la redacción de README.md, FUENTES.md y estructura de carpetas del repositorio.
- Adaptación: La documentación fue ajustada al proyecto SmartPipes, sus componentes reales, conexiones y flujo de datos.
- Comprensión: El equipo entiende la función de cada carpeta, archivo y plataforma dentro del sistema completo.

## Declaración final

El equipo declara que las herramientas externas e inteligencia artificial fueron utilizadas como apoyo para acelerar el desarrollo, depuración y documentación del proyecto. Las decisiones técnicas, integración física, pruebas y adaptación al problema fueron realizadas por el equipo.
