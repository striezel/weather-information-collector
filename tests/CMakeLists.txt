cmake_minimum_required (VERSION 3.8)

# MSYS2 / MinGW uses Catch 3.x.
if (MINGW)
  set(HAS_CATCH_V3 ON)
else ()
  set(HAS_CATCH_V3 OFF)
endif ()

# Some newer distros like Fedora 38+ or Ubuntu 24.04+ also use Catch 3.x.
if (UNIX AND CMAKE_VERSION VERSION_GREATER_EQUAL "3.22.0")
  cmake_host_system_information(RESULT DIST_NAME QUERY DISTRIB_NAME)
  cmake_host_system_information(RESULT DIST_VERSION_ID QUERY DISTRIB_VERSION_ID)

  if (DIST_NAME STREQUAL "Fedora Linux" AND DIST_VERSION_ID VERSION_GREATER_EQUAL "38")
    # Fedora 38 or later detected, set flag for Catch 3.x.
    set(HAS_CATCH_V3 ON)
  endif ()

  if (DIST_NAME STREQUAL "Ubuntu" AND DIST_VERSION_ID VERSION_GREATER_EQUAL "24.04")
    # Ubuntu 24.04 or later detected, set flag for Catch 3.x.
    set(HAS_CATCH_V3 ON)
  endif ()
endif()

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
