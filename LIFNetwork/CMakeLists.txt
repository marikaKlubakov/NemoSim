cmake_minimum_required(VERSION 3.1)

project(LIFNetworkSimulation
		LANGUAGES C CXX)

macro(set_source_group VARIABLE GROUP_NAME)
	set(${VARIABLE} ${ARGN})
	source_group(${GROUP_NAME} FILES ${${VARIABLE}})
endmacro()

set_source_group(SOURCES "Source Files" LIF_neuron.cpp LIFLayer.cpp LIFNetwork.cpp main.cpp )
set_source_group(HEADERS "Header Files" LIF_neuron.h LIFLayer.h LIFNetwork.h )

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
