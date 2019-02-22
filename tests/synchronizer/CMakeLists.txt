cmake_minimum_required (VERSION 2.8)

# #################################################################### #
# tests for weather-information-collector-task-synchronizer executable #
# #################################################################### #

# add tests for --version and --help parameters
# default help parameter "--help"
add_test(NAME weather-information-collector-synchronizer_help
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> --help)

# short help parameter with question mark "-?"
add_test(NAME weather-information-collector-task-creator_help_question_mark
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> -?)

# Windows-style help parameter "/?"
add_test(NAME weather-information-collector-task-creator_help_question_mark_windows
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> /?)

# parameter to show version number
add_test(NAME weather-information-collector-task-creator_version
         COMMAND $<TARGET_FILE:weather-information-collector-synchronizer> --version)
