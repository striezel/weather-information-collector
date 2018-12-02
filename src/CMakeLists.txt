# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

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
