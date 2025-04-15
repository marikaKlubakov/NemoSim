cmake_minimum_required(VERSION 3.1)

project(LIFNetworkSimulation
		LANGUAGES C CXX)

macro(set_source_group VARIABLE GROUP_NAME)
	set(${VARIABLE} ${ARGN})
	source_group(${GROUP_NAME} FILES ${${VARIABLE}})
endmacro()

set_source_group(SOURCES "Source Files" LIFNeuron.cpp LIFLayer.cpp LIFNetwork.cpp main.cpp )
set_source_group(HEADERS "Header Files" LIFNeuron.hpp LIFLayer.hpp LIFNetwork.hpp )

add_executable(LIFNetworkSimulation ${SOURCES} ${HEADERS})

if(UNIX)
	target_link_libraries(LIFNetworkSimulation PRIVATE dl pthread -static-libgcc -static-libstdc++)
endif()

if ( MSVC )
    set_target_properties( LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
    set_target_properties( LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "../")
    set_target_properties( LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "../")
else()
    set_target_properties( LIFNetworkSimulation PROPERTIES RUNTIME_OUTPUT_DIRECTORY "../")
endif ()
