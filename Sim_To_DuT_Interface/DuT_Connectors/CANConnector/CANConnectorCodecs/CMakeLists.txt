cmake_minimum_required(VERSION 3.20)

target_sources(libs
        PRIVATE
        BmwCodec.cpp
        SuzukiCodec.cpp
        PUBLIC
        BmwCodec.h
        SuzukiCodec.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR})