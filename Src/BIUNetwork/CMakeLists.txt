cmake_minimum_required(VERSION 3.5)
set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")

project(BIUNetwork LANGUAGES C CXX)

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
target_include_directories(BIUNetwork PUBLIC ${CMAKE_SOURCE_DIR}/BIUNetwork)