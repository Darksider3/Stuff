cmake_minimum_required(VERSION 3.16)

project(LiGi_Apps VERSION 0.0.1)
# C++20!
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# additional debug flags... ^o^
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++2a -g -pedantic-errors -Wuseless-cast -Wall -Wpedantic -Wextra -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wno-redundant-move -Wno-pessimizing-move -Wnon-virtual-dtor -Wconversion")
# Set include directory

get_filename_component(PARENT_DIR ../../../../ ABSOLUTE)
include_directories(${PARENT_DIR}/)
add_executable(lipsum_gen main.cpp lipsum.cpp lipsum.hpp)

