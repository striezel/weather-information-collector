cmake_minimum_required (VERSION 3.8...3.31)

if (JSON_BENCHMARK)
  # Recurse into subdirectory for the JSON benchmark.
  add_subdirectory (benchmark)
endif (JSON_BENCHMARK)

# Recurse into subdirectory for the main executable.
add_subdirectory (collector)

# Recurse into subdirectory for the configuration test application.
add_subdirectory (configtest)

# Recurse into subdirectory for the task creator application.
add_subdirectory (creator)

# Recurse into subdirectory for the database synchronization application.
add_subdirectory (synchronizer)

# Recurse into subdirectory for the database update application.
add_subdirectory (update)
