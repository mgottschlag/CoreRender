
project(CoreRender)
cmake_minimum_required(VERSION 2.6.0)

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Windows")
if(CMAKE_COMPILER_IS_GNUCXX)
	add_definitions(-std=c++0x)
endif(CMAKE_COMPILER_IS_GNUCXX)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Linux" OR ${CMAKE_SYSTEM_NAME} MATCHES "Windows")

set(CMAKE_BUILD_TYPE Debug)

add_subdirectory(CoreRender)
add_subdirectory(Tutorials)
add_subdirectory(Tools)
add_subdirectory(Tests)
add_subdirectory(Extensions)
