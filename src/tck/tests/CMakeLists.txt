# TCK Tests CMakeLists.txt
# Sets up Google Test and includes test subdirectories

if (APPLE OR WIN32)
    include(FetchContent)
    FetchContent_Declare(googletest
            GIT_REPOSITORY https://github.com/google/googletest.git
            GIT_TAG release-1.12.1)

    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    include(googletest)
else ()
    find_package(GTest CONFIG REQUIRED)
    include(GoogleTest)
endif ()

add_subdirectory(unit)
