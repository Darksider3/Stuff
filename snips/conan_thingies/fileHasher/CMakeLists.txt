cmake_minimum_required(VERSION 3.16)
project(POCO_MD5_Encrypt)

add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

add_executable(fileHash source.cpp)
target_link_libraries(fileHash ${CONAN_LIBS})