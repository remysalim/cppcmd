set(TARGET_NAME ${PROJECT_NAME}-demo)
add_executable(${TARGET_NAME} main.cpp)
target_link_libraries(${TARGET_NAME} cppshell)

target_compile_options(${TARGET_NAME} PRIVATE
  $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX /EHsc>
  $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -pedantic -Werror>
)
