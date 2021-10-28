cmake_minimum_required(VERSION 3.20)
project(Dummy_CAN_DuT C)

set(CMAKE_C_STANDARD 99)

include_directories(include)
add_executable(Dummy_CAN_DuT src/Dummy_CANFD_DuT.c src/Dummy_CANFD_Socket.c)
