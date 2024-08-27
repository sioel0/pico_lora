add_executable(client client.cpp)

target_link_libraries(client pico_stdlib pico_multicore hardware_uart hardware_spi hardware_gpio hardware_timer comm PicoHal RadioLib)

pico_add_extra_outputs(client)

pico_enable_stdio_usb(client 1)
pico_enable_stdio_uart(client 0)
