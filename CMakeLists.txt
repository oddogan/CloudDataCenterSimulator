cmake_minimum_required(VERSION 3.5)

# set the C++ standard to C++ 17
set(CMAKE_CXX_STANDARD 17)

project(CDC_Simulator VERSION 1.0
                      DESCRIPTION "Cloud Data Center Simulation"
                      LANGUAGES CXX)

add_subdirectory(src)