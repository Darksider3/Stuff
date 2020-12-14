cmake_minimum_required(VERSION 3.16)

project(LiGi_Apps VERSION 0.0.1)
add_compile_options(-lbsd)
add_link_options(-lbsd)
# C++20!
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_BUILD_TYPE Debug)

# additional debug flags... ^o^
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++2a -g -pedantic-errors -Wuseless-cast -Wall -Wpedantic -Wextra -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wno-redundant-move -Wno-pessimizing-move -Wnon-virtual-dtor -Wconversion")

# @TODO: NcursesW?
#SET(CURSES_USE_NCURSESW TRUE)

# Include NCurses
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

# Include threads(but prefer PTHREAD, ty)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
if(NOT CMAKE_USE_PTHREADS_INIT)
  message(FATAL_ERROR "Currently i just support pthreads. :/")
endif()

# Set include directory
get_filename_component(PARENT_DIR ../../../ ABSOLUTE)
include_directories(${PARENT_DIR}/)
message("Trace for PARENT_DIR ${PARENT_DIR}")
add_executable(one-time-pad one-time-pad.cpp)

