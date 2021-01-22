cmake_minimum_required(VERSION 3.16)
project(POCO_File_Hasher)

add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

add_executable(fileHash source.cpp FileHashPrinter.hpp common.hpp StringFormat.hpp)
#target_link_libraries(fileHash ${CONAN_LIBS})
conan_target_link_libraries(fileHash)