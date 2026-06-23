require("dotenv").config();

const express = require("express");
const cors = require("cors");
const { Pool } = require("pg");
const path = require("path");

const app = express();
const PORT = process.env.PORT || 3000;

if (!process.env.DATABASE_URL) {
  console.error("ERROR: Falta la variable DATABASE_URL");
  process.exit(1);
}

const pool = new Pool({
  connectionString: process.env.DATABASE_URL,
  ssl: {
    rejectUnauthorized: false
  }
});

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, "public")));

// Crear tabla automaticamente si no existe
async function crearTabla() {
  await pool.query(`
    CREATE TABLE IF NOT EXISTS mediciones (
      id SERIAL PRIMARY KEY,
      nodo_id INTEGER NOT NULL,
      distancia_cm NUMERIC(6,2),
      porcentaje_lleno NUMERIC(5,2),
      humedad BOOLEAN,
      estado TEXT,
      bateria_voltaje NUMERIC(4,2),
      creado_en TIMESTAMP DEFAULT NOW()
    );
  `);

  console.log("Tabla mediciones lista");
}

crearTabla().catch((err) => {
  console.error("Error creando tabla:", err);
});

// Ruta para probar que la API funciona
app.get("/api/status", (req, res) => {
  res.json({
    ok: true,
    mensaje: "SmartPipes API funcionando",
  });
});

// Recibir datos desde la ESP32
app.post("/api/mediciones", async (req, res) => {
  try {
    const {
      nodo_id,
      distancia_cm,
      porcentaje_lleno,
      humedad,
      estado,
      bateria_voltaje,
    } = req.body;

    if (!nodo_id) {
      return res.status(400).json({
        ok: false,
        error: "Falta nodo_id",
      });
    }

    const resultado = await pool.query(
      `
      INSERT INTO mediciones 
      (nodo_id, distancia_cm, porcentaje_lleno, humedad, estado, bateria_voltaje)
      VALUES ($1, $2, $3, $4, $5, $6)
      RETURNING *;
      `,
      [
        nodo_id,
        distancia_cm,
        porcentaje_lleno,
        humedad,
        estado,
        bateria_voltaje || null,
      ]
    );

    res.json({
      ok: true,
      medicion: resultado.rows[0],
    });
  } catch (error) {
    console.error("Error guardando medicion:", error);
    res.status(500).json({
      ok: false,
      error: "Error guardando medicion",
    });
  }
});

// Ultimas mediciones generales
app.get("/api/mediciones", async (req, res) => {
  try {
    const resultado = await pool.query(`
      SELECT *
      FROM mediciones
      ORDER BY creado_en DESC
      LIMIT 200;
    `);

    res.json(resultado.rows);
  } catch (error) {
    console.error("Error leyendo mediciones:", error);
    res.status(500).json({
      ok: false,
      error: "Error leyendo mediciones",
    });
  }
});

// Ultimo estado de cada sensor
app.get("/api/nodos", async (req, res) => {
  try {
    const resultado = await pool.query(`
      SELECT DISTINCT ON (nodo_id)
        nodo_id,
        distancia_cm,
        porcentaje_lleno,
        humedad,
        estado,
        bateria_voltaje,
        creado_en
      FROM mediciones
      ORDER BY nodo_id, creado_en DESC;
    `);

    res.json(resultado.rows);
  } catch (error) {
    console.error("Error leyendo sensores:", error);
    res.status(500).json({
      ok: false,
      error: "Error leyendo sensores",
    });
  }
});

// Historico por sensor y mes
app.get("/api/historico", async (req, res) => {
  try {
    const sensorId = Number(req.query.sensor_id);
    const mes = req.query.mes; // formato YYYY-MM

    if (!sensorId || !mes) {
      return res.status(400).json({
        ok: false,
        error: "Falta sensor_id o mes. Ejemplo: /api/historico?sensor_id=1&mes=2026-06",
      });
    }

    const inicio = `${mes}-01`;

    const resultado = await pool.query(
      `
      SELECT *
      FROM mediciones
      WHERE nodo_id = $1
        AND creado_en >= $2::date
        AND creado_en < ($2::date + INTERVAL '1 month')
      ORDER BY creado_en ASC;
      `,
      [sensorId, inicio]
    );

    res.json({
      ok: true,
      sensor_id: sensorId,
      mes,
      total: resultado.rows.length,
      mediciones: resultado.rows,
    });
  } catch (error) {
    console.error("Error leyendo historico:", error);
    res.status(500).json({
      ok: false,
      error: "Error leyendo historico",
    });
  }
});

app.listen(PORT, () => {
  console.log(`SmartPipes API corriendo en puerto ${PORT}`);
});
