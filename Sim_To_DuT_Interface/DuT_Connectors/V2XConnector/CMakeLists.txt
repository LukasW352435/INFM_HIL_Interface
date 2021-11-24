cmake_minimum_required(VERSION 3.20)

target_sources(libs
        PRIVATE
        V2XConnector.cpp
        PUBLIC
        V2XConnector.h)

#set(CMAKE_PREFIX_PATH /opt/ros/noetic:${CMAKE_PREFIX_PATH} CACHE STRING "")

add_subdirectory(cube-its/catkin_ws/src)

find_package(Vanetza REQUIRED)
target_link_libraries(libs PUBLIC Vanetza::vanetza)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})