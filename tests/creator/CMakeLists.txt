# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# ############################################################### #
# tests for weather-information-collector-task-creator executable #
# ############################################################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME weather-information-collector-task-creator_help
         COMMAND $<TARGET_FILE:weather-information-collector-task-creator> --help)

# short help parameter with question mark "-?"
add_test(NAME weather-information-collector-task-creator_help_question_mark
         COMMAND $<TARGET_FILE:weather-information-collector-task-creator> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
  add_test(NAME weather-information-collector-task-creator_help_question_mark_windows
           COMMAND $<TARGET_FILE:weather-information-collector-task-creator> /?)
else ()
  message(STATUS "Not running creator question mark test on GHA with MinGW.")
endif ()

# parameter to show version number
add_test(NAME weather-information-collector-task-creator_version
         COMMAND $<TARGET_FILE:weather-information-collector-task-creator> --version)
