cmake_minimum_required(VERSION 3.20)
project(DoxyGen)

set(CMAKE_CXX_STANDARD 17)

find_package(Doxygen REQUIRED)
# set input and output files
set(DOXYGEN_IN Doxyfile.in)
set(DOXYGEN_OUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

# request to configure the file
configure_file(${DOXYGEN_IN} ${DOXYGEN_OUT} @ONLY)
message("Doxygen build started")

# note the option ALL which allows to build the docs together with the application
add_custom_target( DoxyGen ALL
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_OUT}
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM )