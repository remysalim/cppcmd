cmake_minimum_required(VERSION 3.10)
project(cppcmd CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_BINARY_DIR})

option(CPPCMD_HAS_STREAM_OPERATORS "Enable usage of stream operators" ON)

add_library(cppcmd INTERFACE)

if (CPPCMD_HAS_STREAM_OPERATORS)
    target_compile_definitions(cppcmd INTERFACE CPPCMD_HAS_STREAM_OPERATORS)
endif()
target_include_directories(cppcmd INTERFACE include)

if (BUILD_TESTING)
    enable_testing()
    add_subdirectory(apps)
    add_subdirectory(tests)
endif()

include(GNUInstallDirs)
install(
    DIRECTORY ${PROJECT_SOURCE_DIR}/include/
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING
    PATTERN "*.hpp"
)
