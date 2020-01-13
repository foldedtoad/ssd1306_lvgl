cmake_minimum_required(VERSION 3.13.1)

set(BOARD nrf52_pca10040)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(display)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE display.c)
target_sources(app PRIVATE fonts/font5x7.c)
target_sources(app PRIVATE fonts/font8x8.c)
target_sources(app PRIVATE fonts/font10x16.c)
target_sources(app PRIVATE fonts/font15x24.c)
target_sources(app PRIVATE fonts/font20x32.c)

target_include_directories(app PRIVATE ./)
