cmake_minimum_required(VERSION 3.5)

if(UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
	set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
endif()
project(LIFNetwork LANGUAGES C CXX)

set(SOURCES 
    YFlash.cpp
)

set(HEADERS 
    YFlash.hpp
)

# Create a static library instead of a shared one
add_library(YFlash STATIC ${SOURCES} ${HEADERS})

# Allow NEMOSIM to use LIFNetwork headers
target_include_directories(LIFNetwork PUBLIC ${CMAKE_SOURCE_DIR}/YFlash)

