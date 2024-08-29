file(GLOB FILES *.cpp *.hh)

add_library(comm ${FILES})

target_link_libraries(comm hardware_uart pico_stdlib)

target_include_directories(comm PUBLIC ../../include/Comm/)

pico_enable_stdio_usb(comm 1)
