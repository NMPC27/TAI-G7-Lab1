cmake_minimum_required(VERSION 3.10)

project(cpm VERSION 1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Output final binaries to the 'bin' folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

add_executable(cpm src/cpm.cpp src/cpm.h)
