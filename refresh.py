import paho.mqtt.client as mqtt
import json

topic = "pot"

# Callback que se llama cuando el cliente recibe el CONNACK del servidor
# Result code 0 significa conexión sin errores
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    # Nos suscribimos al topic
    client.subscribe(topic)

# Callback que se llama "automáticamente" cuando se recibe un mensaje del Publisher.
def on_message(client, userdata, msg):
    msg.payload = msg.payload.decode("utf-8")
    mensaje_recibido = msg.payload
    print("Mensaje recibido " + mensaje_recibido)
    # Deserializamos el JSON
    mensaje_recibido_json = json.loads(msg.payload)
    timestamp = mensaje_recibido_json["timestamp"]
    print(timestamp)
    var_pot = mensaje_recibido_json["value"]
    print(var_pot)

# Creamos un cliente MQTT
client = mqtt.Client()
# Definimos los callbacks para conectarnos y suscribirnos al topic
client.on_connect = on_connect
client.on_message = on_message

hostname = "imanolpi"  # dirección del BROKER. Hostname de mi RPi
client.connect(hostname, 1883, 60)  # 1883 es el puerto por defecto
client.loop_forever()  # Conexión ininterrumpida > el cliente está continuamente escuchando
                        # nuevas publicaciones en el topic
