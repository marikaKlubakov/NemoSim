cmake_minimum_required(VERSION 3.5)

if(UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
	set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
endif()
project(LIFNetwork LANGUAGES C CXX)
include_directories(${CMAKE_SOURCE_DIR}/Src/YFlash)
include_directories(${CMAKE_SOURCE_DIR}/Src/NemoSimEngine)

set(SOURCES 
    LIFNeuron.cpp
    LIFLayer.cpp
    LIFNetwork.cpp
)

set(HEADERS 
    LIFNeuron.hpp
    LIFLayer.hpp
    LIFNetwork.hpp
)

# Create a static library instead of a shared one
add_library(LIFNetwork STATIC ${SOURCES} ${HEADERS})

# Allow NEMOSIM to use LIFNetwork headers
target_include_directories(LIFNetwork PUBLIC ${CMAKE_SOURCE_DIR}/LIFNetwork)

