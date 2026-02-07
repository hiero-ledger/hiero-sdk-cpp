
if(NOT WIN32)
    if(NOT DEFINED RESOLV_LIBRARY)
        find_library(RESOLV_LIBRARY resolv)
    endif()

    add_library(sys::resolv STATIC IMPORTED)
    set_target_properties(sys::resolv PROPERTIES IMPORTED_LOCATION ${RESOLV_LIBRARY})
endif()

 # Linux dependency checks
 if(UNIX AND NOT APPLE)

    # Check for zip utility
    find_program(ZIP_EXECUTABLE zip)

    if(NOT ZIP_EXECUTABLE)
        message(FATAL_ERROR "zip utility is required but not installed. Run: sudo apt install zip")
    endif()

    # Check for linux headers (linux-libc-dev)
    include(CheckIncludeFile)

    set(CMAKE_TRY_COMPILE_SOURCE_EXTENSION c)
    
    check_include_file("linux/version.h" HAVE_LINUX_HEADERS)

    if(NOT HAVE_LINUX_HEADERS)
        message(FATAL_ERROR "linux-libc-dev is required but not installed. Run: sudo apt install linux-libc-dev")
    endif()

endif()

if(APPLE)
    if(NOT DEFINED OSX_CORE_FOUNDATION)
        find_library(OSX_CORE_FOUNDATION CoreFoundation)
    endif()

    if(NOT DEFINED OSX_CF_NETWORK)
        find_library(OSX_CF_NETWORK CFNetwork)
    endif()
endif()
