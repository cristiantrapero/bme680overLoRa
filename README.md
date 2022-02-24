# bme680overLoRa: Send Bosch BME680 sensor data over LoRa RAW
This project allows to transfer Bosch BME680 sensor data over LoRa RAW channel. This is implemented with STM32 L476RG, Bosch BME680, 0.96" OLED screen and 2 Pycom Lopy 4.

# Install pymark in your computer
1. Install NojeJS depending on the operating system you are using: https://nodejs.org/es/download/
2. Install the Pymakr environment for VSCode as described here: https://docs.pycom.io/gettingstarted/software/vscode/

# How to deploy the application
1. Clone this repository: `git clone https://github.com/cristiantrapero/bme680overLoRa.git`
2. Open the `lopy` folder in VSCode
3. Connect the `Lopy 4 sender` to the computer USB
4. List the serial ports with the Pymakr bar: `All commands -> Pymakr -> Extra -> List Serial Ports`
7. Change the `"address": "COM5"` in the pymakr.conf file to your corresponding setial port listed
8. Copy the `lopy/sender.py` code in the `lopy/main.py` file
9. Run the Pymakr command: `Upload`
10. Wait until load the project in the `Lopy 4 sender`
11. Disconnect the `Lopy 4 sender`
12. Connect the `Lopy 4 receiver` to the computer USB
13. Copy the `lopy/receiver.py` code in the `lopy/main.py` file
14. Modify the **WLAN and MQTT parameters**
15. Run the Pymakr command: `Upload`
16. Wait until load the project in the `Lopy 4 receiver`
17. Connect the components as in the image. 
18. Import the `stm32/BME680-OLED-LORA` project in STM32CubeIDE.
19. Compile and upload the stm project to the STM32 L476RG board in **debug mode**.
20. In the Debug view, press **continue button**.

## Files
- `lopy`: Lopy 4 project for sender and reciever node.
- `stm32`: STM32 project.

## Firmware versions
Lopy4 firmware version: 
- Pycom MicroPython: **1.20.2.r6 [v1.11-c5a0a97]** released at 2021-10-28.
