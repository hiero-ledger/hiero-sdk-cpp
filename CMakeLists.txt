cmake_minimum_required(VERSION 3.15...4.3)
project(hiero-sdk-cpp VERSION 0.55.0 DESCRIPTION "Hiero SDK C++" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
    add_compile_options(/bigobj)
endif()

# Linux-only linker optimizations to reduce memory pressure during linking.
# The gRPC 1.76 / protobuf 5.29 static libraries are significantly larger than
# previous versions, which can cause the default GNU ld linker to exceed CI
# runner memory limits. Using mold (a memory-efficient modern linker),
# split-DWARF (to move debug info out of .o files), and gc-sections (to discard
# unused code) brings link-time memory well within budget.
if (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    # Use the mold linker for dramatically lower memory usage
    find_program(MOLD_LINKER "mold")
    if (MOLD_LINKER)
        add_link_options("-fuse-ld=mold")
        message(STATUS "Using mold linker: ${MOLD_LINKER}")
    endif ()

    # Split DWARF: moves debug info into separate .dwo files so ld processes
    # much less data.  Only meaningful for Debug / RelWithDebInfo builds.
    if (CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
        add_compile_options(-gsplit-dwarf)
        message(STATUS "Enabled split-DWARF for ${CMAKE_BUILD_TYPE} build")
    endif ()

    # Place each function and data item in its own section so the linker can
    # discard unreferenced symbols (--gc-sections).
    add_compile_options(-ffunction-sections -fdata-sections)
    add_link_options(-Wl,--gc-sections)
endif ()

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
find_package(libsecp256k1 CONFIG REQUIRED)

include(FetchContent)
include(SystemLibraries.cmake)
include(HieroApi.cmake)

enable_testing()

add_subdirectory(src)
