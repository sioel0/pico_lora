cmake_minimum_required(VERSION 3.12)

include($ENV{PICO_SDK_PATH}/pico_sdk_init.cmake)

project(pico_lora)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

pico_sdk_init()

add_compile_options(
  -Wall
  -Wno-format
  -Wno-unused-function
)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/include/RadioLib" "${CMAKE_CURRENT_BINARY_DIR}/RadioLib")

add_subdirectory(./src)
add_subdirectory(./roles)
