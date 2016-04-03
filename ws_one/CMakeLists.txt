project(ws_one)

cmake_minimum_required(VERSION 3.2)

include(${CMAKE_SOURCE_DIR}/cmake/arduino_uno.cmake)

set( MYPROJECT_SOURCE_FILES
     ${CMAKE_SOURCE_DIR}/WStationOne.cpp
     ${ARDUINO_SOURCE_FILES}
)

add_arduino_lib( "LiquidCrystal" )
add_arduino_lib( "RTCLib" )
add_arduino_lib( "Wire" )
add_arduino_lib( "Adafruit-BMP085" )
add_arduino_lib( "DHT" )

#message( STATUS "****${ARDUINO_SOURCE_FILES}")

add_executable(firmware ${MYPROJECT_SOURCE_FILES})
