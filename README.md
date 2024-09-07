# pico_lora
This project has been created to implement a smart lighting system proof of concept for
"Progettazione di Sistemi Operativi" exam at University of Milan accademic year 2023/2024.

This particular repository is linked with [STM32_Smart_Lighting_System](https://github.com/sioel0/STM32_Smart_Lighting_System)
which is the other part of the project. This repository in particular contains a firmware
for the Raspberry Pi Pico which, with the use of [WaveShare Pico Lora shield](https://www.waveshare.com/wiki/Pico-LoRa-SX1262), is used
to manage all the communication part of the project.
In the roles folder there are two different implementations:
- router is used to send commands to the lighting system from remote
- client is used on the lighting part to receive and send additional data to the router
