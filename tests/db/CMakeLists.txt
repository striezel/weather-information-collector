# We might support earlier versions, too, but it's only tested with 2.8.9.
cmake_minimum_required (VERSION 2.8)

# Recurse into subdirectory for database tests with mysql++.
add_subdirectory (mysqlpp)

# Recurse into subdirectory for database tests with mysql++, part 2.
add_subdirectory (mysqlpp2)

# Recurse into subdirectory for database tests with mariadb library.
add_subdirectory (mariadb)
