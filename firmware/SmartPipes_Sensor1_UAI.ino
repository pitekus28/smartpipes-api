#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

// =====================================================
// SMART PIPES
// ESP32-S3 + SENSOR ULTRASONICO SR04M-2 + SENSOR HUMEDAD
// VERSION SIN OLED
// ENVIA DATOS A DASHBOARD WEB EN RAILWAY
// =====================================================


// =====================================================
// 1. IDENTIFICACION DEL SENSOR
// =====================================================

// Este numero identifica el sensor dentro de la base de datos.
// En el dashboard aparece como "Sensor 1 UAI".
#define NODO_ID 1


// =====================================================
// 2. CALIBRACION FISICA DE LA ALCANTARILLA
// =====================================================

// Distancia medida cuando la alcantarilla esta limpia.
// Como la alcantarilla limpia mide aprox 1 metro,
// dejamos 100 cm como referencia de 0% lleno.
#define DISTANCIA_LIMPIA_CM 100.0

// Distancia medida cuando la alcantarilla esta casi tapada.
// Mientras menor es la distancia, mas cerca esta el agua/basura/sedimento del sensor.
// Dejamos 20 cm como referencia de 100% lleno.
#define DISTANCIA_TAPADA_CM 20.0

/*
  Interpretacion general:

  100 cm o mas = alcantarilla limpia = 0% lleno
  20 cm o menos = alcantarilla tapada = 100% lleno

  El sensor mide distancia desde arriba hacia abajo.

  Si la distancia es grande:
  - El fondo esta lejos.
  - Hay poco material acumulado.
  - El sistema esta normal.

  Si la distancia es chica:
  - El material esta cerca del sensor.
  - Hay acumulacion, agua o basura.
  - El sistema esta en alerta o emergencia.
*/


// =====================================================
// 3. DATOS WIFI
// =====================================================

// Nombre de la red WiFi.
// Ojo: tu red tiene un punto al final.
const char* WIFI_SSID = "iPhone de javier.";

// Clave WiFi.
const char* WIFI_PASSWORD = "123456789";


// =====================================================
// 4. API RAILWAY
// =====================================================

// URL publica de la API.
// La ESP32 envia aqui las mediciones.
// Railway las guarda en la base de datos Aiven.
const char* API_URL = "https://smartpipes-api-production.up.railway.app/api/mediciones";


// =====================================================
// 5. PINES
// =====================================================

// Sensor ultrasonico SR04M-2 / AJ-SR04M:
// Conexion usada:
// 5V sensor  -> 5V/VIN ESP32
// GND sensor -> GND ESP32
// TX sensor  -> GPIO18 ESP32
// RX sensor  -> sin conectar
const int echoPin = 18;


// Sensor humedad / lluvia de 2 pines:
// Conexion recomendada:
// Un pin del sensor -> 3V3
// Otro pin del sensor -> GPIO4
// GPIO4 -> resistencia 10k -> GND
//
// La resistencia 10k es importante porque evita falsos positivos.
// Sin resistencia, GPIO4 queda flotando y puede marcar humedad falsa.
const int rainPin = 4;


// Umbral de humedad.
// Se ajusta mirando el monitor serial.
// Si seco da bajo y mojado da alto, se usa valor > 1500.
// Si seco da alto y mojado da bajo, se invierte en leerLluvia().
#define UMBRAL_LLUVIA 1500


// =====================================================
// 6. ESTRUCTURA DE ESTADO
// =====================================================

// Esta estructura guarda el estado calculado del alcantarillado.
struct Estado {
  String nombre;          // NORMAL, ATENCION, ALERTA, RIESGO, CRITICO, EMERGENCIA, ERROR
  String interpretacion;  // Texto corto explicativo
  int nivel;              // Nivel numerico de gravedad: 0 a 5
};


// =====================================================
// 7. CALCULAR PORCENTAJE DE LLENADO
// =====================================================

float calcularPorcentajeLleno(float distancia) {

  // Si la distancia es negativa, significa error de lectura.
  if (distancia < 0) {
    return -1;
  }

  /*
    Formula:

    porcentaje = (distancia limpia - distancia actual)
                 -------------------------------------- x 100
                 (distancia limpia - distancia tapada)

    Con nuestra calibracion:

    distancia limpia = 100 cm
    distancia tapada = 20 cm

    Ejemplos:

    distancia = 100 cm:
    porcentaje = 0%

    distancia = 60 cm:
    porcentaje = 50%

    distancia = 20 cm:
    porcentaje = 100%
  */

  float porcentaje = ((DISTANCIA_LIMPIA_CM - distancia) /
                     (DISTANCIA_LIMPIA_CM - DISTANCIA_TAPADA_CM)) * 100.0;

  // Si mide mas de 100 cm, puede dar negativo.
  // Lo dejamos como 0%.
  if (porcentaje < 0) {
    porcentaje = 0;
  }

  // Si mide menos de 20 cm, puede dar mas de 100.
  // Lo dejamos como 100%.
  if (porcentaje > 100) {
    porcentaje = 100;
  }

  return porcentaje;
}


// =====================================================
// 8. EVALUAR ESTADO CON DISTANCIA + PORCENTAJE
// =====================================================

Estado evaluarEstado(float distancia, float porcentaje) {
  Estado e;

  /*
    Esta funcion usa DOS criterios:

    1. Distancia fisica en centimetros:
       Indica que tan cerca esta la obstruccion del sensor.

    2. Porcentaje de llenado:
       Indica que tan llena esta la alcantarilla respecto a la calibracion.

    Mientras menor es la distancia, peor.
    Mientras mayor es el porcentaje, peor.
  */

  // Error de sensor.
  if (distancia < 0 || porcentaje < 0) {
    e.nombre = "ERROR";
    e.interpretacion = "Falla sensor";
    e.nivel = -1;
  }

  // EMERGENCIA:
  // Distancia <= 30 cm o porcentaje >= 90%.
  // El material esta muy cerca del sensor.
  else if (distancia <= 30 || porcentaje >= 90) {
    e.nombre = "EMERGENCIA";
    e.interpretacion = "Tapado total";
    e.nivel = 5;
  }

  // CRITICO:
  // Distancia <= 45 cm o porcentaje >= 75%.
  // La alcantarilla esta casi colapsada.
  else if (distancia <= 45 || porcentaje >= 75) {
    e.nombre = "CRITICO";
    e.interpretacion = "Casi colapsado";
    e.nivel = 4;
  }

  // RIESGO:
  // Distancia <= 60 cm o porcentaje >= 60%.
  // Hay obstruccion alta.
  else if (distancia <= 60 || porcentaje >= 60) {
    e.nombre = "RIESGO";
    e.interpretacion = "Obstruccion alta";
    e.nivel = 3;
  }

  // ALERTA:
  // Distancia <= 80 cm o porcentaje >= 40%.
  // Puede existir bloqueo parcial.
  else if (distancia <= 80 || porcentaje >= 40) {
    e.nombre = "ALERTA";
    e.interpretacion = "Posible bloqueo";
    e.nivel = 2;
  }

  // ATENCION:
  // Distancia menor a 100 cm o porcentaje >= 20%.
  // Hay acumulacion leve.
  else if (distancia < 100 || porcentaje >= 20) {
    e.nombre = "ATENCION";
    e.interpretacion = "Acumulacion leve";
    e.nivel = 1;
  }

  // NORMAL:
  // Distancia >= 100 cm y porcentaje menor a 20%.
  else {
    e.nombre = "NORMAL";
    e.interpretacion = "Ducto despejado";
    e.nivel = 0;
  }

  return e;
}


// =====================================================
// 9. MEDIR DISTANCIA UNA VEZ
// =====================================================

float medirDistanciaUnaVez() {

  /*
    pulseIn mide cuanto tiempo el GPIO18 esta en HIGH.

    El sensor ultrasonico calcula distancia con el tiempo
    que demora el pulso en volver.

    Timeout = 35000 microsegundos.
    Si no hay lectura, devuelve 0.
  */

  long duracion = pulseIn(echoPin, HIGH, 35000);

  // Si no hubo pulso, devolvemos error.
  if (duracion == 0) {
    return -1;
  }

  /*
    Velocidad del sonido aprox:
    0.034 cm/us

    El sonido va y vuelve, por eso se divide en 2.

    distancia = duracion x 0.034 / 2
  */

  float distancia = duracion * 0.034 / 2.0;

  return distancia;
}


// =====================================================
// 10. MEDIR DISTANCIA PROMEDIO
// =====================================================

float medirDistanciaPromedio() {

  // Tomamos varias mediciones para evitar lecturas locas.
  const int muestras = 5;

  float suma = 0;
  int validas = 0;

  for (int i = 0; i < muestras; i++) {

    float d = medirDistanciaUnaVez();

    // Aceptamos solo mediciones razonables.
    if (d > 0 && d < 600) {
      suma += d;
      validas++;
    }

    delay(100);
  }

  // Si ninguna medicion fue valida, retornamos error.
  if (validas == 0) {
    return -1;
  }

  // Promedio de lecturas validas.
  return suma / validas;
}


// =====================================================
// 11. SENSOR DE HUMEDAD / LLUVIA
// =====================================================

int leerValorLluvia() {

  // Lee el valor analogico del GPIO4.
  // En ESP32 normalmente va de 0 a 4095.
  return analogRead(rainPin);
}


bool leerLluvia() {

  int valor = leerValorLluvia();

  /*
    Con la conexion:
    sensor -> 3V3
    sensor -> GPIO4
    GPIO4 -> resistencia 10k -> GND

    Lo esperado es:
    - Seco/desconectado: valor bajo
    - Mojado/conduciendo: valor alto

    Por eso usamos:
    valor > UMBRAL_LLUVIA

    Si en tu prueba pasa al reves, cambia la linea por:
    return valor < UMBRAL_LLUVIA;
  */

  return valor > UMBRAL_LLUVIA;
}


// =====================================================
// 12. CONECTAR A WIFI
// =====================================================

bool conectarWiFi() {

  Serial.print("Conectando a WiFi: ");
  Serial.println(WIFI_SSID);

  // Modo estacion: la ESP32 se conecta a una red existente.
  WiFi.mode(WIFI_STA);

  // Inicia conexion.
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int intentos = 0;

  // Espera hasta 15 segundos aprox.
  while (WiFi.status() != WL_CONNECTED && intentos < 30) {
    delay(500);
    Serial.print(".");
    intentos++;
  }

  Serial.println();

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi conectado correctamente");
    Serial.print("IP ESP32: ");
    Serial.println(WiFi.localIP());
    return true;
  } else {
    Serial.println("No se pudo conectar a WiFi");
    return false;
  }
}


// =====================================================
// 13. ENVIAR DATOS A RAILWAY
// =====================================================

void enviarDatosAPI(float distancia, float porcentaje, bool lluvia, String estado) {

  // Si se perdio WiFi, intenta reconectar.
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reintentando...");

    if (!conectarWiFi()) {
      Serial.println("No se pudo enviar porque no hay WiFi");
      return;
    }
  }

  /*
    Como la URL es HTTPS, usamos WiFiClientSecure.

    client.setInsecure() evita problemas de certificado SSL.
    Para prototipo universitario esta bien.
  */

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  Serial.print("Enviando datos a API: ");
  Serial.println(API_URL);

  // Inicia conexion HTTPS.
  http.begin(client, API_URL);

  // Indicamos que el contenido enviado es JSON.
  http.addHeader("Content-Type", "application/json");

  /*
    JSON enviado a la API.

    La base de datos guarda:
    - nodo_id
    - distancia_cm
    - porcentaje_lleno
    - humedad
    - estado
    - bateria_voltaje

    Por ahora bateria_voltaje queda fijo en 3.70.
    Cuando agreguemos medicion real de bateria, se reemplaza.
  */

  String json = "{";
  json += "\"nodo_id\":" + String(NODO_ID) + ",";
  json += "\"distancia_cm\":" + String(distancia, 1) + ",";
  json += "\"porcentaje_lleno\":" + String(porcentaje, 1) + ",";
  json += "\"humedad\":" + String(lluvia ? "true" : "false") + ",";
  json += "\"estado\":\"" + estado + "\",";
  json += "\"bateria_voltaje\":3.70";
  json += "}";

  Serial.println("JSON enviado:");
  Serial.println(json);

  // Envia los datos usando POST.
  int codigoRespuesta = http.POST(json);

  Serial.print("Codigo respuesta API: ");
  Serial.println(codigoRespuesta);

  // Respuesta que manda Railway.
  String respuesta = http.getString();

  Serial.println("Respuesta API:");
  Serial.println(respuesta);

  // Cierra conexion.
  http.end();
}


// =====================================================
// 14. IMPRIMIR EN MONITOR SERIAL
// =====================================================

void imprimirSerial(float distancia, float porcentaje, bool lluvia, int valorLluvia, Estado estado) {

  Serial.println();
  Serial.println("========== SMART PIPES ==========");

  Serial.print("Sensor: ");
  Serial.print(NODO_ID);
  Serial.println(" UAI");

  Serial.println("---------------------------------");

  Serial.print("Distancia fisica medida: ");

  if (distancia < 0) {
    Serial.println("ERROR");
  } else {
    Serial.print(distancia, 1);
    Serial.println(" cm");
  }

  Serial.print("Porcentaje de llenado: ");

  if (porcentaje < 0) {
    Serial.println("ERROR");
  } else {
    Serial.print(porcentaje, 1);
    Serial.println(" %");
  }

  Serial.println("---------------------------------");

  Serial.print("Valor humedad ADC: ");
  Serial.println(valorLluvia);

  Serial.print("Humedad detectada: ");
  Serial.println(lluvia ? "SI" : "NO");

  Serial.println("---------------------------------");

  Serial.print("Estado: ");
  Serial.println(estado.nombre);

  Serial.print("Interpretacion: ");
  Serial.println(estado.interpretacion);

  Serial.print("Nivel visual: ");

  int nivelSeguro = estado.nivel;

  if (nivelSeguro < 0) {
    nivelSeguro = 0;
  }

  if (nivelSeguro > 5) {
    nivelSeguro = 5;
  }

  for (int i = 0; i < nivelSeguro; i++) {
    Serial.print("#");
  }

  for (int i = nivelSeguro; i < 5; i++) {
    Serial.print("-");
  }

  Serial.println();

  Serial.println("---------------------------------");

  Serial.println("Criterio de distancia:");
  Serial.println(">= 100 cm  -> NORMAL");
  Serial.println("< 100 cm   -> ATENCION");
  Serial.println("<= 80 cm   -> ALERTA");
  Serial.println("<= 60 cm   -> RIESGO");
  Serial.println("<= 45 cm   -> CRITICO");
  Serial.println("<= 30 cm   -> EMERGENCIA");

  Serial.println("---------------------------------");

  Serial.println("Criterio de porcentaje:");
  Serial.println("0% - 19%   -> NORMAL");
  Serial.println("20% - 39%  -> ATENCION");
  Serial.println("40% - 59%  -> ALERTA");
  Serial.println("60% - 74%  -> RIESGO");
  Serial.println("75% - 89%  -> CRITICO");
  Serial.println("90% - 100% -> EMERGENCIA");

  Serial.println("=================================");
}


// =====================================================
// 15. SETUP
// =====================================================

void setup() {

  // Inicia comunicacion serial.
  Serial.begin(115200);

  // Espera 1 segundo para que el monitor serial cargue.
  delay(1000);

  // Configura el pin del sensor ultrasonico.
  pinMode(echoPin, INPUT);

  // Configura el pin del sensor de humedad.
  pinMode(rainPin, INPUT);

  Serial.println("=================================");
  Serial.println(" SMART PIPES INICIADO");
  Serial.println(" VERSION SIN OLED");
  Serial.println(" CALIBRACION A 1 METRO");
  Serial.println("=================================");

  Serial.println("Sensor distancia:");
  Serial.println("5V sensor ultrasonico -> 5V/VIN ESP32");
  Serial.println("GND sensor ultrasonico -> GND ESP32");
  Serial.println("TX sensor ultrasonico -> GPIO18 ESP32");
  Serial.println("RX sensor ultrasonico -> sin conectar");

  Serial.println();

  Serial.println("Sensor humedad:");
  Serial.println("Un pin sensor humedad -> 3V3");
  Serial.println("Otro pin sensor humedad -> GPIO4");
  Serial.println("GPIO4 -> resistencia 10k -> GND");

  Serial.println();

  Serial.println("Calibracion fisica:");
  Serial.print(DISTANCIA_LIMPIA_CM);
  Serial.println(" cm o mas = alcantarilla limpia = 0%");
  Serial.print(DISTANCIA_TAPADA_CM);
  Serial.println(" cm o menos = alcantarilla tapada = 100%");

  Serial.println();

  Serial.println("API:");
  Serial.println(API_URL);

  Serial.println("=================================");

  // Intenta conectar a WiFi.
  conectarWiFi();
}


// =====================================================
// 16. LOOP PRINCIPAL
// =====================================================

void loop() {

  // 1. Medir distancia fisica desde el sensor hasta el fondo/obstruccion.
  float distancia = medirDistanciaPromedio();

  // 2. Leer valor crudo del sensor de humedad.
  int valorLluvia = leerValorLluvia();

  // 3. Convertir humedad a SI/NO segun el umbral.
  bool lluvia = leerLluvia();

  // 4. Calcular porcentaje de llenado usando 100 cm como limpio y 20 cm como tapado.
  float porcentaje = calcularPorcentajeLleno(distancia);

  // 5. Evaluar estado usando distancia fisica y porcentaje.
  Estado estado = evaluarEstado(distancia, porcentaje);

  // 6. Mostrar todo en monitor serial.
  imprimirSerial(distancia, porcentaje, lluvia, valorLluvia, estado);

  // 7. Enviar distancia, porcentaje, humedad y estado a Railway.
  enviarDatosAPI(distancia, porcentaje, lluvia, estado.nombre);

  // 8. Esperar 10 segundos.
  // Para pruebas esta bien.
  // Para version con bateria despues usamos deep sleep.
  delay(10000);
}
