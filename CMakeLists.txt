cmake_minimum_required(VERSION 3.5...4.0)
project(NEMOSIM LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 14)

# Compiler settings
if(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
else()
    set(CMAKE_C_COMPILER cl)
    set(CMAKE_CXX_COMPILER cl)
endif()

# Add subdirectories
add_subdirectory(LIFNetwork)
#add_subdirectory(BIUNetwork)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/LIFNetwork)
include_directories(${CMAKE_SOURCE_DIR})

# Define the main executable
set(SOURCES 
    main.cpp
    XMLParser.cpp
    tinyxml2.cpp
)

set(HEADERS 
    tinyxml2.h    
    XMLParser.hpp
)

add_executable(NEMOSIM ${SOURCES} ${HEADERS})

target_link_libraries(NEMOSIM PRIVATE LIFNetwork)

# Ensure correct output naming
set_target_properties(NEMOSIM PROPERTIES OUTPUT_NAME "NEMOSIM")


# Post-build command
add_custom_command(
    TARGET NEMOSIM
    POST_BUILD
    COMMAND python ../Tests/postBuildTester.py
)