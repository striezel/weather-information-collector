# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

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
