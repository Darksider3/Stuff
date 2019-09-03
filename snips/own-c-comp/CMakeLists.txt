cmake_minimum_required(VERSION 3.14)
project(own_c_comp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -DDEBUG")

add_executable(own_c_comp main.cpp)
add_executable(configparser configparse.cpp)