project(ws_one)

cmake_minimum_required(VERSION 3.2)

include(${CMAKE_SOURCE_DIR}/cmake/arduino_uno.cmake)


#add_arduino_lib( "LiquidCrystal" )
add_arduino_lib( "RTCLib" )
add_arduino_lib( "Wire" )
add_arduino_lib( "Adafruit-BMP085" )
add_arduino_lib( "DHT" )
add_arduino_lib( "SoftwareSerial" )

set( MYPROJECT_SOURCE_FILES
     ${CMAKE_SOURCE_DIR}/WStationOne.cpp
     ${ARDUINO_SOURCE_FILES}
)



add_executable(firmware ${MYPROJECT_SOURCE_FILES} )
add_custom_command(
    TARGET firmware
    PRE_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${ARDUINO_ROOT}/hardware/tools/avr/avr/lib/avr5/crtm328p.o
        ${CMAKE_BINARY_DIR}
    COMMENT "Copy object file")

target_link_libraries( firmware "printf_min" )
