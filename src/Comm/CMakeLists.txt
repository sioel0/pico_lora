file(GLOB FILES *.c *.h)

add_library(comm ${FILES})

target_link_libraries(comm hardware_i2c pico_i2c_slave pico_stdlib)

target_include_directories(comm PUBLIC ../../include/Comm/)
