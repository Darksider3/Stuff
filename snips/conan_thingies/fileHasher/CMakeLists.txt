cmake_minimum_required(VERSION 3.16)
project(POCO_File_Hasher)

add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Ofast -DNDEBUG")

include_directories("src")

add_executable(fileHash main.cpp src/Print/Adapters/FileHashPrinter.hpp src/common.hpp src/Print/Format/StringFormat.hpp src/Print/FileIO/ReadIntoEngine.hpp src/DigestEncryptApp.hpp)
#target_link_libraries(fileHash ${CONAN_LIBS})
conan_target_link_libraries(fileHash)