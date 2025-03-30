cmake_minimum_required(VERSION 3.13)

# initialize the SDK directly
include($ENV{PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(E:/ABCV/SDK_s/pico-extras/external/pico_extras_import.cmake)

#seting C and C++ standards
set(CMAKE_C_COMPILER "arm-none-eabi-gcc")
set(CMAKE_CXX_COMPILER "arm-none-eabi-g++")

set(C_STANDARD 11)
set(Cxx_STANDARD 17)

project(MC_Project C CXX ASM)
message("\nProject name is ${PROJECT_NAME}\n")

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()


add_executable(${PROJECT_NAME} main.cpp Liquid_Crystal_RP.cpp Liquid_Crystal_RP.hpp RP2040_DS3231.cpp RP2040_DS3231.hpp)

#target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/includes/LiquidCrystal_i2c)

#add_library(SHARED includes/LiquidCrystal_i2c/Liquid_Crystal_RP.cpp includes/LiquidCrystal_i2c/Liquid_Crystal_RP.hpp)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(${PROJECT_NAME} hardware_i2c pico_stdlib hardware_sleep hardware_irq)

pico_add_extra_outputs(${PROJECT_NAME}) 

# Enable usb output, disable uart output
pico_enable_stdio_usb(${PROJECT_NAME} 1)
pico_enable_stdio_uart(${PROJECT_NAME} 0)