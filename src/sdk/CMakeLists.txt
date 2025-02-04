# Optional Examples build
option(BUILD_EXAMPLES "Build the example builds" OFF)

if(BUILD_EXAMPLES)
    message(STATUS "Including examples in the build.")
    add_subdirectory(examples)
else()
    message(STATUS "Examples are not included in the build.")
endif()
add_subdirectory(main)
# Optional Tests build
option(BUILD_TESTS "Build the test suite" OFF)

if(BUILD_TESTS)
    message(STATUS "Including tests in the build.")
    add_subdirectory(tests)
else()
    message(STATUS "Tests are not included in the build.")
endif()
