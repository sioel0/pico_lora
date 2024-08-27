add_executable(router router.cpp)

target_link_libraries(router pico_stdlib pico_multicore hardware_uart hardware_spi hardware_gpio hardware_timer comm PicoHal RadioLib)

pico_add_extra_outputs(router)

pico_enable_stdio_usb(router 1)
pico_enable_stdio_uart(router 0)
