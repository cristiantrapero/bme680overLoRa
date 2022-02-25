from network import LoRa, WLAN
from mqtt import MQTTClient
import socket
import time
import struct
import pycom
import machine
import time

# Colors
off = 0x000000
red = 0xff0000
green = 0x00ff00
blue = 0x0000ff
pycom.heartbeat(False)

print("Trying to connected to WiFi...\n")

# WLAN parameters (mandatory to set)
ssid = ""
wlan_password = ""

wlan = WLAN(mode=WLAN.STA)
wlan.connect(ssid, auth=(WLAN.WPA2, wlan_password), timeout=10000)

while not wlan.isconnected():
    machine.idle()

print("Connected to WiFi\n")

# MQTT callback
def sub_cb(topic, msg):
   print(msg)

# MQTT parameters (mandatory to set)
mqtt_host = "mqtt3.thingspeak.com"
mqtt_client_ID = ""
mqtt_username = ""
mqtt_password = ""
channel_ID = "1662416"
topic = "channels/" + channel_ID + "/publish"

client = MQTTClient(mqtt_client_ID, mqtt_host, user=mqtt_username, password=mqtt_password, port=1883)

client.set_callback(sub_cb)
client.connect()
client.subscribe(topic=topic)

lora = LoRa(mode=LoRa.LORA, region=LoRa.EU868)
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
s.setblocking(True)

pycom.rgbled(blue)
print("Waiting for data over LoRa...\n")

while True:
    try:
      data = s.recv(20)
      try:
          decoded = struct.unpack('ffff', data)
          pycom.rgbled(green)
          print('Recieved over LoRa: {}'.format(data))
          temp = decoded[0]
          hum = decoded[1]
          press = decoded[2]
          gas = decoded[3]
          print('Decoded data: {} C, {} rH, {} hPa, {} kohms'.format(temp, hum, press, gas))
          payload = "field1={}&field2={}&field3={}&field4={}&status=MQTTPUBLISH".format(temp, hum, press, gas)
          try:
              # This condition is to avoid error data
              if press > 100:
                  client.publish(topic=topic, msg=payload)
          except Exception as ex:
              print("Error publishing mqtt message. Exception:\n")
              print(ex)
              continue
          pycom.rgbled(off)
      except Exception as ex:
          print("Not values from STM32: Exception:\n")
          print(ex)

    except socket.timeout:
      print('No LoRa packet received')
