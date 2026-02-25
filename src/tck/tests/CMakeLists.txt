# TCK Tests CMakeLists.txt
# Sets up Google Test and includes test subdirectories

find_package(GTest CONFIG REQUIRED)
include(GoogleTest)

add_subdirectory(unit)