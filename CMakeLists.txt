cmake_minimum_required(VERSION 3.13.1)

set(BOARD nrf52_pca10040)

include($ENV{ZEPHYR_BASE}/cmake/app/boilerplate.cmake NO_POLICY_SCOPE)
project(display)

target_sources(app PRIVATE main.c)
target_sources(app PRIVATE buttons.c)
target_sources(app PRIVATE display.c)

target_include_directories(app PRIVATE ./)
