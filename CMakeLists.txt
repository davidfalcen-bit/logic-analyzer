cmake_minimum_required(VERSION 3.25)

set(
    LOGIC_ANALYZER_TARGET
    "embedded"
    CACHE STRING
    "Target to build: embedded or host"
)

set_property(
    CACHE LOGIC_ANALYZER_TARGET
    PROPERTY STRINGS
        embedded
        host
)

if(LOGIC_ANALYZER_TARGET STREQUAL "embedded")
    include(pico_sdk_import.cmake)

    project(
        logic_analyzer
        VERSION 0.1.0
        LANGUAGES C CXX ASM
    )
elseif(LOGIC_ANALYZER_TARGET STREQUAL "host")
    project(
        logic_analyzer_host
        VERSION 0.1.0
        LANGUAGES CXX
    )
else()
    message(FATAL_ERROR
        "Unknown LOGIC_ANALYZER_TARGET: ${LOGIC_ANALYZER_TARGET}. "
        "Use 'embedded' or 'host'."
    )
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_CONFIGURATION_TYPES)
    if(NOT CMAKE_BUILD_TYPE)
        set(
            CMAKE_BUILD_TYPE
            Debug
            CACHE STRING
            "Build type"
            FORCE
        )
    endif()

    set_property(
        CACHE CMAKE_BUILD_TYPE
        PROPERTY STRINGS
            Debug
            Release
            RelWithDebInfo
            MinSizeRel
    )
endif()

if(LOGIC_ANALYZER_TARGET STREQUAL "embedded")
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)

    if(PICO_SDK_VERSION_STRING VERSION_LESS "2.2.0")
        message(FATAL_ERROR
            "Pico SDK 2.2.0 or later required. "
            "Current version: ${PICO_SDK_VERSION_STRING}"
        )
    endif()

    pico_sdk_init()
    add_subdirectory(embedded)
else()
    add_subdirectory(host)
endif()