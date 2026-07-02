cmake_minimum_required(VERSION 3.25)

include(pico_sdk_import.cmake)


set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 20)

project(logic_analyzer
    VERSION 0.1.0
    LANGUAGES C CXX ASM
)

if (PICO_SDK_VERSION_STRING VERSION_LESS "2.2.0")
    message(FATAL_ERROR "You should be using Raspberry Pi Pico SDK version 2.2.0 (or later). Your version is ${PICO_SDK_VERSION_STRING}")
endif()

pico_sdk_init()
add_subdirectory(embedded)