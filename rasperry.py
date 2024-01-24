import serial
import time
import csv
import json
import paho.mqtt.publish as publish
from datetime import datetime

uart_serial = serial.Serial('/dev/ttyUSB0',115200, timeout=10,
parity=serial.PARITY_NONE,
stopbits=serial.STOPBITS_ONE,
bytesize=serial.EIGHTBITS)

topic = "pot"
hostname = "imanolpi"  

# Función para obtener el timestamp actual
def get_current_timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S")

# Función para escribir en el archivo CSV en la ubicación especificada
def write_to_csv(data):
    with open('C:\\Users\\Hezitzaile\\Desktop\\ImanolAitor_entegablefinal\\Imanol_Aitor.csv', 'a', newline='') as file:
        writer = csv.writer(file)
        writer.writerow([data, get_current_timestamp()])

# Función para publicar el mensaje recibido vía MQTT
def publish_message(mensaje):
    mensaje_json = json.dumps({"timestamp": get_current_timestamp(), "value": mensaje})
    publish.single(topic=topic, payload=mensaje_json, qos=1, hostname=hostname)
    print("Mensaje publicado vía MQTT: " + mensaje_json)

if uart_serial.is_open:
    while True:
        size = uart_serial.inWaiting()
        if size:
            data = uart_serial.read(size)
            received_data_string = data.decode('utf-8')
            print(received_data_string)
            write_to_csv(received_data_string)
            publish_message(received_data_string)  # Publicar el mensaje vía MQTT
        else:
            time.sleep(1)
else:
    print('Puerto serie no abierto')

