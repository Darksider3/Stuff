cmake_minimum_required(VERSION 3.14)
project(Cryptopals)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG -lbsd -Iinclude/")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -lbsd -Iinclude/")
include_directories(include/)
add_executable(challenge1 set1/challenge1.cpp)
