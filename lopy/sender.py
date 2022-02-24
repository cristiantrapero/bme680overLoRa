from network import LoRa
from machine import UART
import socket
import time
import struct
import uos
import pycom
import ubinascii

# Colors
off = 0x000000
red = 0xff0000
green = 0x00ff00
blue = 0x0000ff

pycom.rgbled(red)

# Please pick the region that matches where you are using the device
lora = LoRa(mode=LoRa.LORA, region=LoRa.EU868)
s = socket.socket(socket.AF_LORA, socket.SOCK_RAW)
s.setblocking(False)

# UART communication with STM32
uart=UART(1, 115200)
uart.init(115200, bits=8, parity=None, stop=1, pins=('P3', 'P4'))

def send_data_over_lora(temp, hum, press, gas):
    data = struct.pack('ffff', temp, hum, press, gas)
    pycom.rgbled(green)
    s.send(data)
    time.sleep(0.5)
    pycom.rgbled(off)
    print('Sent over LoRa: {}'.format(data))

print("Waiting for UART data...\n")

while True:
    # If there are any UART data
    if(uart.any()>0):
        buf=bytearray(30)
        uart.readinto(buf, 30)

        # Convert to float
        temp = float(buf.decode()[0:5])
        hum = float(buf.decode()[6:12])
        press = float(buf.decode()[13:20])
        gas = float(buf.decode()[21:26])
        print('Received from UART: {} C, {} rH, {} hPa, {} kohms'.format(temp, hum, press, gas))

        send_data_over_lora(temp, hum, press, gas)
