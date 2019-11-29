cmake_minimum_required(VERSION 3.14)
project(CourseraAlgorithms1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG -lbsd")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -lbsd")

add_executable(01QuickFindUF.cpp Week1/slides/01QuickFindUF.cpp)
add_executable(02QuickUnionLazy Week1/slides/02QuickUnionLazy.cpp)
add_executable(03QuickUnionLazyWeighted Week1/slides/03QuickUnionLazyWeighted.cpp)
add_executable(009RandomizedQueue Week2/slides/009RandomizedQueue.cpp)
add_executable(01LinkedStackOfStrings Week2/slides/01LinkedStackOfStrings.cpp)
