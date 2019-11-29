cmake_minimum_required(VERSION 3.14)
project(Courses)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG -lbsd")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -lbsd")

add_subdirectory(Algorithms1Coursera)
add_subdirectory(cryptopals)
