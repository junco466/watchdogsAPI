from flask import Flask, request, jsonify
from flask_cors import CORS
from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi
from datetime import datetime, timezone
import os
from dotenv import load_dotenv
load_dotenv()

app = Flask(__name__)
CORS(app)  # Permite peticiones desde App Inventor o el ESP32

# Conexión a MongoDB
uri = os.getenv("MONGO_URI")  # lee desde variable de entorno
client = MongoClient(uri, server_api=ServerApi('1'))  # Cambia a tu URI si es remoto
db = client["ubicaciones_db"]
collection = db["datos_ubicacion"]

# Ruta POST: recibe lat, lon desde ESP32
@app.route('/api/ubicacion', methods=['POST'])

def recibir_ubicacion():
    data = request.json
    if not data or 'lat' not in data or 'lon' not in data:
        return jsonify({"error": "Datos incompletos"}), 400

    nueva_ubicacion = {
        "lat": data['lat'],
        "lon": data['lon'],
        "timestamp": datetime.now(timezone.utc)  # Con zona horaria UTC
    }
    collection.insert_one(nueva_ubicacion)
    return jsonify({"mensaje": "Ubicación guardada"}), 201

# Ruta GET: última ubicación para App Inventor
@app.route('/api/ubicacion', methods=['GET'])
def obtener_ultima_ubicacion():
    ultima = collection.find_one(sort=[("timestamp", -1)])
    if ultima:
        return jsonify({
            "lat": ultima["lat"],
            "lon": ultima["lon"],
            "timestamp": ultima["timestamp"].isoformat()
        })
    else:
        return jsonify({"error": "No hay ubicaciones registradas"}), 404

#This is just for development
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
