cmake_minimum_required(VERSION 3.14)
project(own_c_comp)

set(CMAKE_CXX_STANDARD 17)

add_executable(own_c_comp main.cpp)
add_executable(configparser configparse.cpp)