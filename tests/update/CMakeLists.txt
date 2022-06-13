# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

# ######################################################### #
# tests for weather-information-collector-update executable #
# ######################################################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME weather-information-collector-update_help
         COMMAND $<TARGET_FILE:weather-information-collector-update> --help)

# short help parameter with question mark "-?"
add_test(NAME weather-information-collector-update_help_question_mark
         COMMAND $<TARGET_FILE:weather-information-collector-update> -?)

# Windows-style help parameter "/?"
if (NOT DEFINED ENV{GITHUB_ACTIONS} OR NOT MINGW)
  add_test(NAME weather-information-collector-update_help_question_mark_windows
           COMMAND $<TARGET_FILE:weather-information-collector-update> /?)
else ()
  message(STATUS "Not running updater question mark test on GHA with MinGW.")
endif ()

# parameter to show version number
add_test(NAME weather-information-collector-update_version
         COMMAND $<TARGET_FILE:weather-information-collector-update> --version)
