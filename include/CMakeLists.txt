# This option is a reminder that we might want to provide an `operator<<` free implementation in a following version.
option(CPPSHELL_HAS_STREAM_OPERATORS "Enable usage of stream operators" ON)

add_library(cppshell INTERFACE)

if (CPPSHELL_HAS_STREAM_OPERATORS)
  target_compile_definitions(cppshell INTERFACE CPPSHELL_HAS_STREAM_OPERATORS)
endif()

target_include_directories(cppshell INTERFACE .)