cmake_minimum_required(VERSION 3.5)
if(UNIX)
    set(CMAKE_C_COMPILER "/usr/bin/gcc")
    set(CMAKE_CXX_COMPILER "/usr/bin/g++")
else()
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
	set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
endif()
project(BIUNetwork LANGUAGES C CXX)
include_directories(${CMAKE_SOURCE_DIR}/Src/DS)

set(SOURCES 
    BIUNeuron.cpp
    BIULayer.cpp
    BIUNetwork.cpp
    EnergyTable.cpp
)

set(HEADERS 
    BIUNeuron.hpp
    BIULayer.hpp
    BIUNetwork.hpp
    EnergyTable.hpp
)

# Create a static library instead of a shared one
add_library(BIUNetwork STATIC ${SOURCES} ${HEADERS})

# Allow NEMOSIM to use BIUNetwork headers
target_include_directories(BIUNetwork PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries(BIUNetwork PRIVATE DS)

# Link math library on Unix systems (needed for std::exp)
if(UNIX)
    target_link_libraries(BIUNetwork PRIVATE m)
endif()