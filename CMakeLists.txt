file(GLOB SOURCES "*.cpp")

add_library(Liquid_Crystal_RP STATIC ${SOURCES})
target_include_directories(Liquid_Crystal_RP PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(Liquid_Crystal_RP PUBLIC hardware_i2c
                                                pico_stdlib
                                                )