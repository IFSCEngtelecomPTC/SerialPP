cmake_minimum_required(VERSION 3.21)
project(SerialPP)

set(CMAKE_CXX_STANDARD 14)

add_library(SerialPP SHARED Serial.cpp)
