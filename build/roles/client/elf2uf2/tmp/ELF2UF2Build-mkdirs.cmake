# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/sioel0/pico/pico-sdk/tools/elf2uf2"
  "/home/sioel0/1_Projects/pico_lora/build/elf2uf2"
  "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2"
  "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/tmp"
  "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/src/ELF2UF2Build-stamp"
  "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/src"
  "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/src/ELF2UF2Build-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/src/ELF2UF2Build-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/sioel0/1_Projects/pico_lora/build/roles/client/elf2uf2/src/ELF2UF2Build-stamp${cfgdir}") # cfgdir has leading slash
endif()
