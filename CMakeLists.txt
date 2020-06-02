cmake_minimum_required(VERSION 3.10)
project(cppshell CXX)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(include)
add_subdirectory(apps)

include(CTest)
add_subdirectory(tests)
