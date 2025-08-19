from flask import Flask, request, jsonify
from flask_cors import CORS
from pymongo.mongo_client import MongoClient
from pymongo.server_api import ServerApi
from datetime import datetime, timezone
import os
from dotenv import load_dotenv
from werkzeug.security import generate_password_hash, check_password_hash

app = Flask(__name__)
CORS(app)  # Permite peticiones desde App Inventor o el ESP32

#DEV MODE ENV VAR
if os.environ.get("FLASK_ENV") == "development":
    from dotenv import load_dotenv
    load_dotenv()

# Conexión a MongoDB
uri = os.getenv("MONGO_URI")  # lee desde variable de entorno
client = MongoClient(uri, server_api=ServerApi('1'))  # Cambia a tu URI si es remoto
db = client["ubicaciones_db"]
collection_ubicacion = db["datos_ubicacion"]
collection_users = db["users"]

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
    collection_ubicacion.insert_one(nueva_ubicacion)
    return jsonify({"mensaje": "Ubicación guardada"}), 201

# Ruta GET: última ubicación para App Inventor
@app.route('/api/ubicacion', methods=['GET'])
def obtener_ultima_ubicacion():
    ultima = collection_ubicacion.find_one(sort=[("timestamp", -1)])
    if ultima:
        return jsonify({
            "lat": ultima["lat"],
            "lon": ultima["lon"],
            "timestamp": ultima["timestamp"].isoformat()
        })
    else:
        return jsonify({"error": "No hay ubicaciones registradas"}), 404

##-------------------------USERS API-----------------------

# Ruta para registrar usuarios
@app.route('/api/register', methods=['POST'])
def registrar_usuario():
    data = request.get_json()

    user = data.get("user")
    password = data.get("password")

    if not user or not password:
        return jsonify({"status": "error", "mensaje": "Datos incompletos"}), 400

    # user si ya existe el usuario
    if collection_users.find_one({"user": user}):
        return jsonify({"status": "error", "mensaje": "El usuario ya existe"}), 409

    # Guardar con hash
    hash_password = generate_password_hash(password)
    collection_users.insert_one({
        "user": user,
        "password": hash_password
    })

    return jsonify({"status": "ok", "mensaje": "Usuario registrado con éxito"}), 201


# Ruta para login
@app.route('/api/login', methods=['POST'])
def verificar_usuario():
    data = request.get_json()

    user = data.get("user")
    password = data.get("password")

    if not user or not password:
        return jsonify({"status": "error", "mensaje": "Datos incompletos"}), 400

    # Buscar usuario
    user = collection_users.find_one({"user": user})

    if user and check_password_hash(user["password"], password):
        return jsonify({"status": "ok", "mensaje": "Login exitoso"}), 200
    else:
        return jsonify({"status": "error", "mensaje": "Usuario o contraseña incorrectos"}), 401


#This is just for development
if __name__ == '__main__':
    app.run(debug=True, host='0.0.0.0', port=5000)
