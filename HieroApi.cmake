set(HAPI_VERSION_TAG "v0.57.0" CACHE STRING "Use the configured version tag for the Hiero API protobufs")

if (HAPI_VERSION_TAG STREQUAL "")
    set(HAPI_VERSION_TAG "v0.57.0")
endif ()

# Fetch the protobuf definitions
FetchContent_Declare(
        HProto
        GIT_REPOSITORY https://github.com/hashgraph/hedera-services.git
        GIT_TAG ${HAPI_VERSION_TAG}
)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(HProto)

# Clean and update the protobuf definitions
set(PROTO_SRC ${PROJECT_SOURCE_DIR}/proto)

# Get all .proto files in the specified directory
file(GLOB_RECURSE PROTO_FILES ${PROTO_SRC}/*.proto)

# List of files to exclude from removal
set(EXCLUDE_FILES
    "consensus_service.proto"
    "mirror_network_service.proto"
    "transaction_list.proto"
)

# Create a list to hold files to be removed
set(FILES_TO_REMOVE "")

# Iterate over all found .proto files
foreach(PROTO_FILE ${PROTO_FILES})
    # Get the file name without the path
    get_filename_component(FILENAME ${PROTO_FILE} NAME)
    
    # Check if the file is in the exclusion list
    list(FIND EXCLUDE_FILES ${FILENAME} INDEX)
    
    # If the file is not in the exclusion list, add it to the removal list
    if (INDEX EQUAL -1)
        list(APPEND FILES_TO_REMOVE ${PROTO_FILE})
    endif()
endforeach()

# Remove the files that are not in the exclusion list
if (FILES_TO_REMOVE)
    file(REMOVE ${FILES_TO_REMOVE})
endif()

file(INSTALL ${PROJECT_SOURCE_DIR}/proto/service-external-proto/mirror/ DESTINATION ${PROTO_SRC}/mirror)
file(INSTALL ${hproto_SOURCE_DIR}/hapi/hedera-protobufs/services/ DESTINATION ${PROTO_SRC})
file(INSTALL ${PROJECT_SOURCE_DIR}/proto/service-external-proto/sdk/ DESTINATION ${PROTO_SRC})

add_subdirectory(proto)

set(HAPI_ROOT_DIR ${CMAKE_CURRENT_BINARY_DIR}/proto)

if (NOT EXISTS ${HAPI_ROOT_DIR})
    message(FATAL_ERROR "Failed to the HAPI_ROOT_DIR at `${HAPI_ROOT_DIR}`")
endif ()

set(HAPI_INCLUDE_DIR ${HAPI_ROOT_DIR})
set(HAPI_LIB_DIR ${HAPI_ROOT_DIR})
