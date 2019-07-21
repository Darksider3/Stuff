cmake_minimum_required(VERSION 3.14)
project(Courses)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG -lbsd ")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -lbsd")

add_executable(01QuickFindUF.cpp Algorithms1Coursera/Week1/slides/01QuickFindUF.cpp)
add_executable(02QuickUnionLazy Algorithms1Coursera/Week1/slides/02QuickUnionLazy.cpp)
add_executable(03QuickUnionLazyWeighted Algorithms1Coursera/Week1/slides/03QuickUnionLazyWeighted.cpp)
add_executable(009RandomizedQueue Algorithms1Coursera/Week2/slides/009RandomizedQueue.cpp)
add_executable(01LinkedStackOfStrings Algorithms1Coursera/Week2/slides/01LinkedStackOfStrings.cpp)