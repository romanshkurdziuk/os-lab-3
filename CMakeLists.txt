cmake_minimum_required(VERSION 3.14)
project(OperatingSystemsLab3 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Threads REQUIRED)

add_subdirectory(libs/Utils)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)