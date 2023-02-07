# Generated Cmake Pico project file

cmake_minimum_required(VERSION 3.13)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# Initialise pico_sdk from installed location
# (note this can come from environment, CMake cache etc)
# set(PICO_SDK_PATH "/home/jabba/pico-sdk")

set(PICO_BOARD pico CACHE STRING "Board type")

# Pull in Raspberry Pi Pico SDK (must be before project)
include(pico_sdk_import.cmake)

if (PICO_SDK_VERSION_STRING VERSION_LESS "1.4.0")
  message(FATAL_ERROR "Raspberry Pi Pico SDK version 1.4.0 (or later) required. Your version is ${PICO_SDK_VERSION_STRING}")
endif()

project(pico-touchscr-sdk-test C CXX ASM)

# Initialise the Raspberry Pi Pico SDK
pico_sdk_init()

# Add executable. Default name is the project name, version 0.1

add_executable(pico-touchscr-sdk-test)

target_sources(pico-touchscr-sdk-test PUBLIC
	${CMAKE_CURRENT_LIST_DIR}/lib/assert.c
        ${CMAKE_CURRENT_LIST_DIR}/ili9341/ili9341.c
        ${CMAKE_CURRENT_LIST_DIR}/touch/msp2807_touch.c
        ${CMAKE_CURRENT_LIST_DIR}/touch/msp2807_calibration.c
        ${CMAKE_CURRENT_LIST_DIR}/test.c
        )

pico_set_program_name(pico-touchscr-sdk-test "pico-touch-sdk-test")
pico_set_program_version(pico-touchscr-sdk-test "0.9")

pico_enable_stdio_uart(pico-touchscr-sdk-test 1)
pico_enable_stdio_usb(pico-touchscr-sdk-test 0)

# Add the standard include files to the build
target_include_directories(pico-touchscr-sdk-test PRIVATE
  ${CMAKE_CURRENT_LIST_DIR}
  ${CMAKE_CURRENT_LIST_DIR}/.. # for our common lwipopts or any other standard includes, if required
)

# Add any user requested libraries
target_link_libraries(
        pico-touchscr-sdk-test
        pico_stdlib
        hardware_spi
        pico_sync
        hardware_timer
        hardware_clocks
        hardware_spi
        )

pico_add_extra_outputs(pico-touchscr-sdk-test)
