cmake_minimum_required(VERSION 3.14)

project(LiGi_Apps VERSION 0.0.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --std=c++2a -g -pedantic-errors -Wuseless-cast -Wall -Wpedantic -Wextra -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wno-redundant-move -Wno-pessimizing-move -Wnon-virtual-dtor -Wconversion")
SET(CURSES_USE_NCURSES TRUE)
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)


find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)


add_executable(Pomocurses timer.cpp)

target_link_libraries(Pomocurses ${CURSES_LIBRARIES} Threads::Threads)
