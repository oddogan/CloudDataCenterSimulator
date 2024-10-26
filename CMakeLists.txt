cmake_minimum_required(VERSION 3.13)

# set the C++ standard to C++ 17
set(CMAKE_CXX_STANDARD 17)

project(CDCSimulator VERSION 1.0
                      DESCRIPTION "Cloud Data Center Simulation"
                      LANGUAGES CXX)

add_compile_options("-Wno-deprecated-declarations")

add_executable(CDCSimulator "")

add_subdirectory(lib)
add_subdirectory(src)
