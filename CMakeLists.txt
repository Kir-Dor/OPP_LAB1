cmake_minimum_required(VERSION 4.0)
project(OPP_LAB1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)

add_executable(lab1 main.cpp)
target_include_directories(lab1 PUBLIC src)