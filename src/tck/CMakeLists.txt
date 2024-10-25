set(TCK_SERVER_NAME ${PROJECT_NAME}-tck)
add_executable(${TCK_SERVER_NAME}
        src/account/AccountService.cc
        src/json/JsonRpcException.cc
        src/json/JsonUtils.cc
        src/key/KeyService.cc
        src/sdk/SdkClient.cc
        src/main.cc
        src/TckServer.cc)

target_link_libraries(${TCK_SERVER_NAME} ${PROJECT_NAME})

target_include_directories(${TCK_SERVER_NAME} PRIVATE include)

install(TARGETS ${TCK_SERVER_NAME} RUNTIME DESTINATION tck)
