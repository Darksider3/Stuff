get_filename_component(PARENT_DIR ../../../../ ABSOLUTE)
include_directories(${PARENT_DIR}/)
add_executable(lipsum_gen main.cpp lipsum.cpp lipsum.hpp)

