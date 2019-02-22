cmake_minimum_required (VERSION 2.8)

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
add_test(NAME weather-information-collector-benchmark_help_question_mark_windows
         COMMAND $<TARGET_FILE:weather-information-collector-benchmark> /?)

# parameter to show version number
add_test(NAME weather-information-collector-configtest_version
         COMMAND $<TARGET_FILE:weather-information-collector-benchmark> --version)
