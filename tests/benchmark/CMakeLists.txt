cmake_minimum_required (VERSION 3.8...3.31)

# ############################################################ #
# tests for weather-information-collector-benchmark executable #
# ############################################################ #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME weather-information-collector-benchmark_help
         COMMAND $<TARGET_FILE:weather-information-collector-benchmark> --help)

# short help parameter with question mark "-?"
add_test(NAME weather-information-collector-benchmark_help_question_mark
         COMMAND $<TARGET_FILE:weather-information-collector-benchmark> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
  add_test(NAME weather-information-collector-benchmark_help_question_mark_windows
           COMMAND $<TARGET_FILE:weather-information-collector-benchmark> /?)
else ()
  message(STATUS "Not running benchmark question mark test on GHA with MinGW.")
endif ()

# parameter to show version number
add_test(NAME weather-information-collector-benchmark_version
         COMMAND $<TARGET_FILE:weather-information-collector-benchmark> --version)

# tests for parameter misuse
if (NOT WIN32)
  set(EXT "sh")
else ()
  set(EXT "cmd")
endif()

add_test(NAME weather-information-collector-benchmark_parameter_misuse
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse.${EXT} $<TARGET_FILE:weather-information-collector-benchmark>)
