cmake_minimum_required(VERSION 3.16)
project(POCO_File_Hasher)


SET(SRCS main.cpp
        main.cpp
        src/common.hpp
        src/Print/Adapters/FileHashPrinter.hpp
        src/Print/Format/StringFormat.hpp
        src/Print/Format/AbstractOutputFormatter.hpp
        src/Print/FileIO/ReadIntoEngine.hpp src/DigestEncryptApp.hpp
        )


add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Og")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Ofast -DNDEBUG")

include_directories("src")

add_executable(fileHash "${SRCS}")
#target_link_libraries(fileHash ${CONAN_LIBS})
conan_target_link_libraries(fileHash)