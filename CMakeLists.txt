cmake_minimum_required(VERSION 3.20)

project(Diploma)

# find_package(Qt6 REQUIRED COMPONENTS Core)

# qt_standard_project_setup()

set(CMAKE_CXX_STANDARD 17)

set(INCLUDE_DIR ${CMAKE_SOURCE_DIR}/include)
set(RESOURCES ${CMAKE_SOURCE_DIR}/src)

add_subdirectory(src)
add_subdirectory(tests)

