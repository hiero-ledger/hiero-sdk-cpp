find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_subdirectory(unit)
add_subdirectory(integration)
