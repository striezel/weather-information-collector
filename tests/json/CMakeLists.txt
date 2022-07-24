# We might support earlier versions, too, but try to use a more recent one.
cmake_minimum_required (VERSION 3.8)

try_compile(CAN_HAVE_SIMDJSON
            ${CMAKE_CURRENT_SOURCE_DIR}/simd-test
            ${CMAKE_CURRENT_SOURCE_DIR}/../../src/simdjson-compile-test
            json-parse-simd-test
)

if (CAN_HAVE_SIMDJSON)
  message(STATUS "JSON parsing tests for simdjson can be enabled.")
  # Recurse into subdirectory for simdjson tests.
  add_subdirectory (simdjson)
else()
  message(STATUS "Compiler is not fit for simdjson, won't include simdjson parsing tests.")
endif(CAN_HAVE_SIMDJSON)

# Recurse into subdirectory for nlohmann/json tests.
add_subdirectory (nlohmannjson)

# Recurse into subdirectory for generic tests.
add_subdirectory (generic)
