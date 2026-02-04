# TCK Unit Tests CMakeLists.txt
# Creates a separate test executable for TCK (Test Compatibility Kit)
# TCK tests are independent from SDK tests

find_package(httplib CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Set the test executable name
set(TCK_TEST_NAME ${PROJECT_NAME}-tck-tests)

# Create test executable with TCK sources (excluding main.cc)
add_executable(${TCK_TEST_NAME}
        TckServerUnitTests.cc
        ../../src/TckServer.cc
        ../../src/json/JsonRpcException.cc
        ../../src/json/JsonUtils.cc
        ../../src/key/KeyService.cc)

# Include directories for TCK headers and SDK headers (for Transaction.h dependency)
target_include_directories(${TCK_TEST_NAME} PRIVATE
        ../../include
        ${PROJECT_SOURCE_DIR}/src/sdk/main/include)

# Link against required libraries (including SDK for TCK dependencies)
target_link_libraries(${TCK_TEST_NAME} PRIVATE
        gtest_main
        gtest
        httplib::httplib
        nlohmann_json::nlohmann_json
        ${PROJECT_NAME})

# Discover and register tests with CTest
gtest_discover_tests(${TCK_TEST_NAME} WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_BUILD_TYPE})
