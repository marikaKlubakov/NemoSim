cmake_minimum_required(VERSION 3.5)

if(UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
	set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
endif()
project(DS LANGUAGES C CXX)

set(SOURCES 
    DS.cpp
)

set(HEADERS 
    DS.hpp
)

# Create a static library instead of a shared one
add_library(DS STATIC ${SOURCES} ${HEADERS})

# Allow NEMOSIM to use DS headers
target_include_directories(DS PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

