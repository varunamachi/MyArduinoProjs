project(MYPROJECT)

cmake_minimum_required(VERSION 3.2)

include(${CMAKE_SOURCE_DIR}/scripts/arduino_uno.cmake)

set(MYPROJECT_SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/main.cpp
    ${ARDUINO_SOURCE_FILES}
)

add_executable(firmware ${MYPROJECT_SOURCE_FILES})
