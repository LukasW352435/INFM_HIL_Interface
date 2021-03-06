cmake_minimum_required(VERSION 3.18)
project(REST_Dummy_DuT CXX)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

find_package(CURL REQUIRED)
link_directories(CURL::libcurl)
set(CMAKE_CXX_STANDARD 17)
set(BUILD_TESTS OFF CACHE BOOL "Build unit tests.")
include_directories(restbed/source generatedAPI/api generatedAPI/model)
add_subdirectory(restbed)
add_executable(${PROJECT_NAME} main.cpp
        generatedAPI/api/DefaultApi.cpp generatedAPI/api/DefaultApi.h
        generatedAPI/model/Message.cpp generatedAPI/model/Message.h
        apiImpl/DefaultApiImpl.cpp apiImpl/DefaultApiImpl.h
        messages/InterfaceMessages.cpp messages/InterfaceMessages.h
        messages/MessageWithTimestamp.cpp messages/MessageWithTimestamp.h
        apiImpl/RegisteredInterface.cpp apiImpl/RegisteredInterface.h)
target_link_libraries(${PROJECT_NAME} PRIVATE restbed-static PRIVATE CURL::libcurl)
