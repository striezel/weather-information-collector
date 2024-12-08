cmake_minimum_required (VERSION 3.8...3.31)

include (CheckIncludeFileCXX)
check_include_file_cxx (catch2/catch_test_macros.hpp HAS_CATCH_V3_HEADER)
check_include_file_cxx (catch2/catch.hpp HAS_CATCH_V2_HEADER)

if (HAS_CATCH_V3_HEADER AND NOT HAS_CATCH_V2_HEADER)
  set(HAS_CATCH_V3 ON)
else ()
  set(HAS_CATCH_V3 OFF)
endif ()

# Recurse into subdirectory for Apixu tests.
add_subdirectory (api/apixu)

# Recurse into subdirectory for DarkSky tests.
add_subdirectory (api/darksky)

# Recurse into subdirectory for OpenWeatherMap tests.
add_subdirectory (api/owm)

# Recurse into subdirectory for OpenWeatherMap location parsing tests.
add_subdirectory (api/owm-location)

# Recurse into subdirectory for Weatherbit tests.
add_subdirectory (api/weatherbit)

# Recurse into subdirectory for Weatherstack tests.
add_subdirectory (api/weatherstack)

# Recurse into subdirectory for JSON (non-)parsing tests.
add_subdirectory (json)

if (JSON_BENCHMARK)
  # Recurse into subdirectory for benchmark executable tests.
  add_subdirectory (benchmark)
endif (JSON_BENCHMARK)

# Recurse into subdirectory for database tests.
add_subdirectory (db)

# Recurse into subdirectory for executable tests.
add_subdirectory (collector)

# Recurse into subdirectory for configtest executable tests.
add_subdirectory (configtest)

# Recurse into subdirectory for creator executable tests.
add_subdirectory (creator)

# Recurse into subdirectory for general test suite.
add_subdirectory (general)

# Recurse into subdirectory for synchronizer executable tests.
add_subdirectory (synchronizer)

# Recurse into subdirectory for update executable tests.
add_subdirectory (update)

# Recurse into subdirectory for TaskManager tests.
add_subdirectory (task-manager)
