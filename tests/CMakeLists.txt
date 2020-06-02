set(TARGET_NAME ${PROJECT_NAME}-test)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Catch2 REQUIRED)
include(Catch)

add_executable(${TARGET_NAME} test_main.cpp)
target_link_libraries(${TARGET_NAME} cppshell)
target_compile_options(${TARGET_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /EHsc>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)

catch_discover_tests(${TARGET_NAME})