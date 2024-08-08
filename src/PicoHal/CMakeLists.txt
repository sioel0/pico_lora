file(GLOB FILES *.cpp *.h)

add_library(PicoHal ${FILES})

target_link_libraries(PicoHal pico_stdlib hardware_spi hardware_timer RadioLib)

target_include_directories(PicoHal PUBLIC ../../include/PicoHal/)
