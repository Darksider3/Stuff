cmake_minimum_required(VERSION 3.16)

project(CLSR_Things VERSION 0.0.1)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# C++20!
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# additional debug flags... ^o^
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++2a -g -pedantic-errors -Wuseless-cast -Wall -Wpedantic -Wextra -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wno-redundant-move -Wno-pessimizing-move -Wnon-virtual-dtor -Wconversion")

# @TODO: NcursesW?
#SET(CURSES_USE_NCURSESW TRUE)

# Include NCurses
#find_package(Curses REQUIRED)
#include_directories(${CURSES_INCLUDE_DIR})

# Include threads(but prefer PTHREAD, ty)
#set(THREADS_PREFER_PTHREAD_FLAG ON)
#find_package(Threads REQUIRED)
#if(NOT CMAKE_USE_PTHREADS_INIT)
#  message(FATAL_ERROR "Currently i just support pthreads. :/")
#endif()

# Set include directory
#get_filename_component(PARENT_DIR ../../.. ABSOLUTE)
#include_directories(${PARENT_DIR}/)

add_executable(insertion-sort insertion_sort.cpp)

#target_link_libraries(Pomocurses ${CURSES_LIBRARIES} Threads::Threads)
#file(RENAME build/compile_commands.json compile_commands.json)
