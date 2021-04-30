cmake_minimum_required(VERSION 3.16)
project(mathparser)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE debug)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20")

# DEBUG: set(CMAKE_VERBOSE_MAKEFILE on)

include_directories(SYSTEM src/)
SET(SRCS
        src/ShuntingYard.cpp
        src/Token.cpp
        src/RPNEvaluate.cpp)

SET(SanitizerFlags "  -fsanitize=leak -fsanitize=integer-divide-by-zero -fsanitize=unreachable -fsanitize=return  -fsanitize-address-use-after-scope -fsanitize=signed-integer-overflow -fsanitize=float-divide-by-zero -fsanitize=float-cast-overflow -fsanitize=pointer-overflow -fsanitize=builtin")


add_definitions("-std=c++20")

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -Wall -Wextra -Wno-attributes")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fsanitize=undefined -Og -g3 ${SanitizerFlags}")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Ofast -DNDEBUG -march=native -mtune=native")

add_compile_options()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


add_executable(mathparser main.cpp "${SRCS}")

conan_target_link_libraries(mathparser)
