# ssd1306_zephyr

## Overview
This project implements a SSD1306 OLED display connected to a nRF52 PCA10040 (board: nrf52dk_nrf52832) board running Zephyr version 2.4.
In theory, this project could be ported to other Zephyr-supported boards, but this has not (yet) been tried.

Also this project shows how to include custom fonts.

## Hardware
See photo in *docs* directory for view of hardware configuration.
Below is a list hardware components.

 * Nordic PCA10040 Eval Board (nrf52dk_nrf52832)
 * Adafruit 128x32 I2C OLED display (SSD1306)
 * Bread board
 * Ribbon cable with male headers. 
 * (optional) Adafruit pshield board. (ProtoShield v6)

Wire connections as follows

 * P0.27 <--> SCL
 * P0.26 <--> SDA
 * P0.02 <--> RST  (optional)
 * VDD   <--> Vin
 * GND   <--> GND

## Software
This project was built with Zephyr 2.4 and selects the Nordic PCA10040 board ().  
Change the "`set(BOARD nrf52dk_nrf52832)`" in the CMakeFile.txt for other supported boards. 

The following componets need to be configured though "make menuconfig".  

 * Drivers->i2c: I2C-master support for your board/SOC.
 * Drivers->display: Display + SSD1306
 * Subsys->fb: FrameBuffer (cfb)
 * General Kernel Options->Other Kernel Object Option: Heap Memory Pool Size

The tricky part of the software configuration seems to be in setting up the **.overlay* file and the *prj.conf* file.  This requires some knowledge about the build-configuration process for zephyr.  
Hopefully, this project can be used as an example of how to navigate though this procedure.

The difficult parts are setting up the I2C driver for the target board. In this project the nRF52 I2C support is well-known and relatively straight-forward in configuring.  Other SOCs and boards are not always so easy.


## Operation
On the Nordic nRF52832 (PCA10040) board, the four buttons are assigned the following actions:
* Button1 -- Rotates the currently displayed page: 1-4 pages
* Button2 -- Within the currently displayed page, rotate though the editable fields.
* Button3 -- Within the current field, increase the value by step side. (see display.c ~line 101-118)
* Button4 -- Within the current field, decrease the value by step side.  

Screens (Pages) --
* Pg1 -- Shows slider widget.  Use Button3 or Button4 to move slider.
* Pg2 -- Shows two editable fields and an icon (pacman). Use Button3 or Button4 to change field value.
* Pg3 -- Shows three labled fields. Use Button2 to select field.  Use Button3 or Button4 to change value.
* Pg4 -- Shows only a simple graphic icon (zombie eye).
