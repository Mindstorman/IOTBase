import paho.mqtt.client as mqtt
import json
import board
import neopixel

pixels = neopixel.NeoPixel(board.D18,1)

def on_connect(client, userdata, flags, rc):
	print("Connected with result code "+str(rc))
	client.subscribe("lights/set")

def on_message(client, userdata, msg):
	print(msg.topic+" "+str(msg.payload))
	data = json.loads(msg.payload)
	#print("TESTING")	
	#print(data["color"]["r"])
	pixels[0] = (data["color"]["r"],data["color"]["g"],data["color"]["b"])

client = mqtt.Client()
client.username_pw_set("MQTT_USERNAME","MQTT_PASSWORD")
client.on_connect = on_connect
client.on_message = on_message

client.connect("MQTT_BROKER_IP")
client.loop_forever()
