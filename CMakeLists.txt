cmake_minimum_required(VERSION 3.15...3.24)
project(hiero-sdk-cpp VERSION 0.41.0 DESCRIPTION "Hiero SDK C++" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/bigobj)
endif()

set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/${CMAKE_BUILD_TYPE}/${CMAKE_HOST_SYSTEM_NAME}/${CMAKE_HOST_SYSTEM_PROCESSOR})

set(Protobuf_USE_STATIC_LIBS ON)

set(gRPC_SSL_PROVIDER package)

find_package(OpenSSL REQUIRED)
find_package(ZLIB REQUIRED)
find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(re2 CONFIG REQUIRED)
find_package(absl CONFIG REQUIRED)
find_package(Threads REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(EXPAT CONFIG REQUIRED)

include(FetchContent)
include(SystemLibraries.cmake)
include(HieroApi.cmake)

enable_testing()

add_subdirectory(src)
