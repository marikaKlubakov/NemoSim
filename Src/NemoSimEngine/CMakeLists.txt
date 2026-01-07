cmake_minimum_required(VERSION 3.5...4.0)
project(NEMOSIM LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

# Compiler settings
if(WIN32)
    set(CMAKE_C_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
    set(CMAKE_CXX_COMPILER "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/VC/Tools/MSVC/14.29.30133/bin/Hostx64/x64/cl.exe")
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/Src/LIFNetwork)
include_directories(${CMAKE_SOURCE_DIR}/Src/BIUNetwork)
include_directories(${CMAKE_SOURCE_DIR}/Src/ANNNetwork)
include_directories(${CMAKE_SOURCE_DIR}/Src/YFlash)
include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/Src/Common)

# Define the main executable
set(SOURCES 
    main.cpp
    ../Common/XMLParser.cpp
    ../Common/tinyxml2.cpp
    ../Common/BaseNetwork.cpp
    NEMOEngine.cpp
)

set(HEADERS 
    ../Common/tinyxml2.h    
    ../Common/XMLParser.hpp
    ../Common/BaseNetwork.hpp
    networkParams.hpp
    NEMOEngine.hpp
)

add_executable(NEMOSIM ${SOURCES} ${HEADERS})

target_link_libraries(NEMOSIM PRIVATE LIFNetwork BIUNetwork ANNNetwork YFlash)

# Ensure correct output naming

set_target_properties(NEMOSIM PROPERTIES 
    OUTPUT_NAME "NEMOSIM"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/_Build64/"
)



# Post-build command
