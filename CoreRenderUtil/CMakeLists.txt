
project(CoreRenderUtils)
cmake_minimum_required(VERSION 2.6.0)

set(SRC
	include/CoreRenderUtil.hpp
	include/CoreRender/util/FrameLimiter.hpp
	src/util/FrameLimiter.cpp
)

include_directories(include)
include_directories(../CoreRender/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRenderUtil STATIC ${SRC})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRenderUtil SHARED ${SRC})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(CoreRenderUtil CoreRender)

install(DIRECTORY include/CoreRender DESTINATION include/CoreRender FILES_MATCHING PATTERN *.hpp)
install(FILES include/CoreRenderUtil.hpp DESTINATION include)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	install(TARGETS CoreRenderUtil LIBRARY DESTINATION lib)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
