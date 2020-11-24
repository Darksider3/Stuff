cmake_minimum_required(VERSION 3.16)

project(LiGi_Apps VERSION 0.0.1)

# C++20!
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(${PARENT_DIR}/)
add_subdirectory(src/)
