cmake_minimum_required (VERSION 3.8...3.31)

# ############################################################### #
# tests for weather-information-collector-synchronizer executable #
# ############################################################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME weather-information-collector-synchronizer_help
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> --help)

# short help parameter with question mark "-?"
add_test(NAME weather-information-collector-synchronizer_help_question_mark
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
  add_test(NAME weather-information-collector-synchronizer_help_question_mark_windows
           COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> /?)
else ()
  message(STATUS "Not running synchronizer question mark test on GHA with MinGW.")
endif ()

# parameter to show version number
add_test(NAME weather-information-collector-synchronizer_version
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> --version)

# tests for parameter misuse
if (NOT WIN32)
  set(EXT "sh")
else ()
  set(EXT "cmd")
endif()

add_test(NAME weather-information-collector-synchronizer_parameter_misuse
         COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/parameter-misuse.${EXT} $<TARGET_FILE:weather-information-collector-synchronizer>)
