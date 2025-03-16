# BEN-WTR-APP
This is the main application firmware for the BEN-WTR system

## BEN-WTR
The vision is for a system that will measure and post pressure, temperature, and humidity readings to a local MQTT server every minute or so. The firmware should support OTA updates. The system should be able to host a local webpage for configuration.

## HARDWARE
The schematic of breakout boards used can be found at https://github.com/beedeebob/BEN-WTR-HW
- Blue pill DIY board https://stm32-base.org/boards/STM32F103C8T6-Blue-Pill.html modified with STM32F303CBT6 processor (pin compatible)
- BME280 breakout board on SPI, measuring pressure, humidity, and temperature
- W25QXX breakout board on SPI, as external flash storage for firmware OTA updates
- ESP-01 with custom firmware (the latest AT command firmware with MQTT support did not fit) 
