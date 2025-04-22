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
add_subdirectory(BIUNetwork)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/LIFNetwork)
include_directories(../vcpkg/installed/x64-windows/include)

# Define the main executable
set(SOURCES 
    main.cpp
    XMLParser.cpp
)

set(HEADERS 
    XMLParser.hpp
)

add_executable(NEMOSIM ${SOURCES} ${HEADERS})

# Integrate vcpkg
set(CMAKE_TOOLCHAIN_FILE "../vcpkg/scripts/buildsystems/vcpkg.cmake")

# Find and link TinyXML-2
find_package(tinyxml2 CONFIG REQUIRED)
target_link_libraries(NEMOSIM PRIVATE tinyxml2::tinyxml2 LIFNetwork)

# Ensure correct output naming
set_target_properties(NEMOSIM PROPERTIES OUTPUT_NAME "NEMOSIM")

# Post-build command
add_custom_command(
    TARGET NEMOSIM
    POST_BUILD
    COMMAND python ../Tests/postBuildTester.py
)
