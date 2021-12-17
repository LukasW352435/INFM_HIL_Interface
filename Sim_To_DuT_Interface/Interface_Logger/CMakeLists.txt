cmake_minimum_required(VERSION 3.20)

add_subdirectory(quill)

target_sources(libs
        PRIVATE
        InterfaceLogger.cpp
        ../Events/EventVisitor.cpp
        PUBLIC
        InterfaceLogger.h
        InterfaceLoggerConfig.h
        ../Events/EventVisitor.h)

target_include_directories(libs PUBLIC ${CMAKE_CURRENT_LIST_DIR} quill/quill/include)