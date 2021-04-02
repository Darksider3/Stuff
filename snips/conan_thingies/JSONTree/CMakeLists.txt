cmake_minimum_required(VERSION 3.16)
project(JSONTree)

# DEBUG: set(CMAKE_VERBOSE_MAKEFILE on)

include_directories("src")
SET(SRCS
        src/Parsers/MarkdownLinkOptional.hpp
        src/Parsers/MarkdownLinkOptional.cpp
        src/common.hpp
        src/Formatters/Links/LinkFormat.hpp
        src/Formatters/Links/MarkdownLink.hpp
        src/Formatters/Links/DokuwikiLink.hpp
        src/Formatters/Links/HTMLLink.hpp
        src/Formatters/List/MarkdownIndexList.hpp
        src/Formatters/List/IndexList.hpp
        src/Formatters/Links/OptionalMarkdownLink.hpp
        src/Parsers/MarkdownScanner.hpp )

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


add_executable(jsontree main.cpp "${SRCS}")
add_executable(TestLex Tests/main.cpp src/Parsers/StackingMarkdownLexer.cpp src/Parsers/StackingMarkdownLexer.hpp)
#target_link_libraries(fileHash ${CONAN_LIBS})
#target_precompile_headers(jsontree PUBLIC ${SRCS})
#target_precompile_headers(TestLex REUSE_FROM jsontree)

conan_target_link_libraries(jsontree)
conan_target_link_libraries(TestLex)
