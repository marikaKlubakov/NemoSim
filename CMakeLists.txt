cmake_minimum_required(VERSION 3.1)
set(CMAKE_CXX_STANDARD 14)

if(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
else()
    set(CMAKE_C_COMPILER cl)
    set(CMAKE_CXX_COMPILER cl)
endif()


add_subdirectory(LIFNetwork)