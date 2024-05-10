cmake_minimum_required(VERSION 3.12)

# Pull in SDK (must be before project)
include(pico_sdk_import.cmake)

include(pico_extras_import_optional.cmake)

project(project1 C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialize the SDK
pico_sdk_init()

#add_compile_options(-Wall
#        -Wno-format          # int != int32_t as far as the compiler is concerned because gcc has int32_t as long int
#        -Wno-unused-function # we have some for the docs that aren't called
#        )
#if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
#    add_compile_options(-Wno-maybe-uninitialized)
#endif()

add_executable(project1
        src/adc.c
        src/comm.c
        src/display.c
        src/graphics.c
        src/main.c
        include/adc.h
        include/comm.h
        include/display.h
        include/graphics.h
        include/hwconf.h
        )

target_include_directories(project1 PRIVATE include)

# pull in common dependencies
target_link_libraries(project1 pico_stdlib hardware_spi hardware_dma hardware_adc)

# create map/bin/hex file etc.
pico_add_extra_outputs(project1)
