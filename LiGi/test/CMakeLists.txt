cmake_minimum_required(VERSION 3.16)

project(LIGI_TESTS VERSION 0.0.1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_BUILD_TYPE Debug)
set("CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} --std=c++2a -g -pedantic-errors -Wuseless-cast -Wall -Wpedantic -Wextra -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wno-redundant-move -Wno-pessimizing-move -Wnon-virtual-dtor -Wconversion")


get_filename_component(PARENT_DIR ../ ABSOLUTE)
include_directories(${PARENT_DIR})

add_executable(test xdg_basedirs.cpp)
