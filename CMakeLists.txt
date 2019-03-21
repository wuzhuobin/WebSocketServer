cmake_minimum_required(VERSION 3.1)
get_filename_component(
    FOLDER_NAME
    ${CMAKE_SOURCE_DIR}
    NAME_WE
)

project(${FOLDER_NAME})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
if(UNIX)
    set(CMAKE_CXX_FLAGS "-std=c++11 -static")
endif()
add_subdirectory(src)