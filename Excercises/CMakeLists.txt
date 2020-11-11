cmake_minimum_required(VERSION 3.14)
project(Excercises VERSION 0.01
		DESCRIPTION "Excercises"
        LANGUAGES   CXX)


set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall -g -DDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall")
add_compile_options(-lbsd)
add_link_options(-lbsd)
add_subdirectory(joergegonzalez_beginner_projects/)
add_subdirectory(CLRS)
