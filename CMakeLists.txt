
cmake_minimum_required(VERSION 3.10)

project(sunset)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED true)

add_executable(sunset 
    src/main.cpp 
    src/gpio.h src/gpio.cpp 
    src/helper.h src/helper.cpp
    src/net.h src/net.cpp
)

set_target_properties(sunset PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
