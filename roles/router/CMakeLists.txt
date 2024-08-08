add_executable(router router.cpp)

target_link_libraries(router pico_stdlib hardware_uart hardware_spi hardware_gpio hardware_timer hardware_i2c pico_i2c_slave comm PicoHal RadioLib)

pico_add_extra_outputs(router)

pico_enable_stdio_usb(router 1)
pico_enable_stdio_uart(router 1)
