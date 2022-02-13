# FlexiaGloves

This is a fork of the popular project [LucidGloves](https://github.com/LucidVR/lucidgloves). This project aims to build a compact and OpenGlove compatible, flex sensor based VR glove. The hardware is a complete departure from the orignal LucidGlove, but the firmware is largely the same and is fully compatible with device API that the OpenGlove Driver provides.

# Hardware
The hardware of this project differs from LucidGlove because it relies on flex sensors instead potentiometers for finger curl tracking.

## Glove parts
* 3D printed parts in the hardware folder
* 2mm wide nylon zipties | [Amazon](https://www.amazon.com/gp/product/B083ZXC71B)
* 0.4mm spring steel wire | [Amazon](https://www.amazon.com/gp/product/B096X52958)
* MG90 Servos | [Amazon](https://www.amazon.com/s?k=mg90+servo)
* Blue breadboard potentiometers | [Amazon](https://www.amazon.com/HUAREW-Adjustable-Resistance-Potentiometer-Assortment/dp/B0877BKG4R)
* ESP32 WROOM Board | [Amazon](https://www.amazon.com/s?k=esp32)
* 1000 grit sandpaper | [Amazon](https://www.amazon.com/s?k=1000+grit+sandpaper)
* Super Glue | [Amazon](https://www.amazon.com/s?k=super+glue)
* 1/2in Velcro strap | [Amazon](https://www.amazon.com/Fasten-Management-Organizer-Reusable-Gripping/dp/B07GTV7PJK)
* Iron-on fabric patch | [Amazon](https://www.amazon.com/Bondex-Iron-Patches-X7-Pkg-Black/dp/B005UT628U)
* Copper Foil Tape | [Amazon](https://www.amazon.com/gp/product/B09C1ZLFZF)
* AdaFruit Velostat | [Adafruit](https://www.adafruit.com/product/1361)

**Amazon Links are simply examples and other sources/parts may be used**

# Firmware
Follow the instruction from LucidGloves, except use the firmware files in this repository.

Open the firmware files located in the firmware/lucidgloves-firmware folder.
By default the firmware is configured for an arduino nano using serial.
To configure the firmware, change the defines in the lucidgloves-firmware.ino file.

**Configuration instructions are here: [Configuration Instructions](https://github.com/LucidVR/lucidgloves/wiki/Firmware-Setup-and-Customization-Tutorial/).**

## Tested boards:
* Arduino Nano
* ESP-WROOM-32

## Supported Communication Methods:
* USB Serial
* Bluetooth Serial (On ESP32 boards)

# SteamVR Compatibility (OpenGloves)
This project uses the OpenGloves OpenVR driver for compatibility with SteamVR, which is downloadable on Steam:
https://store.steampowered.com/app/1574050/OpenGloves/

# Credits
### Upstream repository available on GitHub:
https://github.com/LucidVR/opengloves-driver

### Special Thanks:
* [LucasVRTech](https://github.com/lucas-vrtech) of the LucidGlove project
* [danwillim](https://github.com/danwillm) of OpenGlove project
* [TwidgeVR](https://github.com/TwidgeVR) guides on how to make homemade flex sensors
