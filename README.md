# bme680overLoRa: Send Bosch BME680 sensor data over LoRa RAW
This project allows to transfer Bosch BME680 sensor data over LoRa RAW channel. This is implemented with STM32 L476RG, Bosch BME680, 0.96" OLED screen and 2 Pycom Lopy 4.

# Install pymark in your computer
1. Install ATOM: https://atom.io/
2. Install the Pymakr plugin for ATOM as described here: https://docs.pycom.io/gettingstarted/software/atom/

# How to deploy the application
1. Clone this repository: `git clone https://github.com/cristiantrapero/bme680overLoRa.git`
2. Open the `lopy` folder in ATOM.
3. Connect the `Lopy 4 sender` to the computer USB.
4. Copy the `lopy/sender.py` code in the `lopy/main.py` file.
5. Press the `Upload project to device` button or crtl+alt+s to upload the project to the Lopy.
6. Wait until load the project in the `Lopy 4 sender`.
7. Disconnect the `Lopy 4 sender`.
8. Connect the `Lopy 4 receiver` to the computer USB.
9. Copy the `lopy/receiver.py` code in the `lopy/main.py` file.
10. Modify the **WLAN and MQTT parameters** in the code.
11. Press the `Upload project to device` button or crtl+alt+s to upload the project to the Lopy.
12. Wait until load the project in the `Lopy 4 receiver`.
13. Connect the components as in the image. 
14. Import the `stm32/BME680-OLED-LORA` project in STM32CubeIDE.
15. Compile and upload the STM project to the STM32 L476RG board in **debug mode**.
16. In the Debug view, press **continue button**.
17. Enjoy!

# Thingspeak
In order to publish the BME680 sensor data in Thingspeak platform, is mandatory:
1. Create a New Channel with 4 fields: https://es.mathworks.com/help/thingspeak/collect-data-in-a-new-channel.html
2. Create a New MQTT device: https://es.mathworks.com/help/thingspeak/mqtt-basics.html#mw_0bf68abc-6c4e-4e2e-8312-e8223a203b71
3. Configure the MQTT parameters in the Lopy: https://es.mathworks.com/help/thingspeak/use-arduino-client-to-publish-to-a-channel.html

## Files
- `lopy`: Lopy 4 project for sender and reciever node.
- `stm32`: STM32 project.
- `showroom.stl`: Showroom 3D model 

## Firmware versions
Lopy4 firmware version: 
- Pycom MicroPython: **1.20.2.r6 [v1.11-c5a0a97]** released at 2021-10-28.
