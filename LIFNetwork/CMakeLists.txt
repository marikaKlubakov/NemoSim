cmake_minimum_required(VERSION 3.1)

project(LIFNetworkSimulation
        LANGUAGES C CXX)

macro(set_source_group VARIABLE GROUP_NAME)
    set(${VARIABLE} ${ARGN})
    source_group(${GROUP_NAME} FILES ${${VARIABLE}})
endmacro()



set(tinyxml2_DIR "../vcpkg/installed/x64-windows/share/tinyxml2")


set_source_group(SOURCES "Source Files" ../XMLParser.cpp LIFNeuron.cpp LIFLayer.cpp LIFNetwork.cpp main.cpp )
set_source_group(HEADERS "Header Files" ../XMLParser.hpp LIFNeuron.hpp LIFLayer.hpp LIFNetwork.hpp )

add_executable(LIFNetworkSimulation ${SOURCES} ${HEADERS})

# Integrate vcpkg
set(CMAKE_TOOLCHAIN_FILE "${CMAKE_SOURCE_DIR}/vcpkg/scripts/buildsystems/vcpkg.cmake")

# Find TinyXML-2
find_package(tinyxml2 CONFIG REQUIRED)

# Link TinyXML-2 library
target_link_libraries(LIFNetworkSimulation PRIVATE tinyxml2::tinyxml2)

if(UNIX)
    target_link_libraries(LIFNetworkSimulation PRIVATE dl pthread -static-libgcc -static-libstdc++)
endif()

if (MSVC)
    set_target_properties(LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
    set_target_properties(LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "../")
    set_target_properties(LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "../")
else()
    set_target_properties(LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
endif()
