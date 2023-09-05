cmake_minimum_required(VERSION 3.20.0)

set(SHIELD ssd1306_128x32)
set(BOARD nrf52dk_nrf52832)

find_package(Zephyr)
project(display)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE buttons.c)
target_sources(app PRIVATE display.c)
target_sources(app PRIVATE icon1.c)
target_sources(app PRIVATE icon2.c)
target_sources(app PRIVATE icon3.c)

target_include_directories(app PRIVATE ./)

zephyr_compile_options(-save-temps)
