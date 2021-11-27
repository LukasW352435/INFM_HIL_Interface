cmake_minimum_required(VERSION 3.20)

add_subdirectory(quill)

target_sources(libs
        PRIVATE
        DuTLogger.cpp
        EventVisitor.cpp
        PUBLIC
        DuTLogger.h
        DuTLoggerConfig.h
        EventVisitor.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR} quill/quill/include)