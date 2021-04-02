cmake_minimum_required(VERSION 3.16)
project(POCO_File_Hasher)

# DEBUG: set(CMAKE_VERBOSE_MAKEFILE on)

SET(SRCS main.cpp
        main.cpp
        src/common.hpp
        src/Print/Adapters/FileHashOut.hpp
        src/Print/Format/StringFormat.hpp
        src/Print/Format/AbstractOutputFormatter.hpp
        src/Print/FileIO/ReadIntoEngine.hpp src/DigestEncryptApp.hpp
        src/Print/Format/PrintFormat.hpp src/Print/Format/CSVFormat.hpp src/Print/Format/BSDFormat.hpp)


#SET(SanitizerFlags "  -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=return  -fsanitize-address-use-after-scope -fsanitize=signed-integer-overflow -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=pointer-overflow -fsanitize=builtin")


add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -Og -g3 ${SanitizerFlags}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Ofast -DNDEBUG")

add_compile_options()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("src")

add_executable(fileHash "${SRCS}")
#target_link_libraries(fileHash ${CONAN_LIBS})
conan_target_link_libraries(fileHash)
