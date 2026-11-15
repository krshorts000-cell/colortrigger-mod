cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")
set(CMAKE_CXX_VISIBILITY_PRESET hidden)

project(ColorTriggerGen VERSION 1.0.0)

# ─── Geode SDK ───────────────────────────────────────────────────────────────
# Set GEODE_SDK in your environment or pass it on the cmake command line:
#   cmake .. -DGEODE_SDK="C:/path/to/geode-sdk"
if (NOT DEFINED ENV{GEODE_SDK})
    if (NOT DEFINED GEODE_SDK)
        message(FATAL_ERROR "Environment variable GEODE_SDK is not set. "
                "Either set it globally or pass -DGEODE_SDK=<path> to CMake.")
    endif()
else()
    set(GEODE_SDK $ENV{GEODE_SDK})
endif()

add_subdirectory(${GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# ─── Sources ─────────────────────────────────────────────────────────────────
file(GLOB_RECURSE SOURCES src/*.cpp src/*.hpp)

add_library(${PROJECT_NAME} SHARED ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    src/
    ${GEODE_SDK}/loader/include
)

setup_geode_mod(${PROJECT_NAME})
