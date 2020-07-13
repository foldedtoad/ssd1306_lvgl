# ssd1306_zephyr

## Overview
This project implements a SSD1306 OLED display connected to a nRF52 PCA10040 (board: nrf52dk_nrf52832) board running Zephyr version 2.3.99
In theory, this project could be ported to other Zephyr-supported boards, but this has not (yet) been tried.

Also this project shows how to include custom fonts.

## Hardware
See photo in *docs* directory for view of hardware configuration.
Below is a list hardware components.

 * Nordic PCA10040 Eval Board (nrf52dk_nrf52832)
 * Adafruit 128x32 I2C OLED display (SSD1306)
 * Bread board
 * Ribbon cable with male headers. 

Wire connections as follows

 * P0.27 <--> SCL
 * P0.26 <--> SDA
 * P0.02 <--> RST  (optional)
 * VDD   <--> Vin
 * GND   <--> GND

## Software
This project was built with Zephyr 2.3.99 and selects the Nordic PCA10040 board ().  
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
The display.c program is rather simple, in that it runs a loop which writes out four different strings to the display: see the *display_play()* function.

