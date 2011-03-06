
project(CoreRenderRocket)
cmake_minimum_required(VERSION 2.6.0)

set(SRC
	include/CoreRender/rocket/FileInterface.hpp
	include/CoreRender/rocket/RenderInterface.hpp
	include/CoreRender/rocket/RocketRenderable.hpp
	include/CoreRender/rocket/RocketTexture.hpp
	include/CoreRender/rocket/SystemInterface.hpp
	src/RenderInterface.cpp
	src/RocketRenderable.cpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(include ../../CoreRender/include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRenderRocket STATIC ${SRC})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRenderRocket SHARED ${SRC})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(CoreRenderRocket ${LIB})

# Installation of files

install(DIRECTORY include/CoreRender DESTINATION include FILES_MATCHING PATTERN *.hpp)
install(FILES include/CoreRenderRocket.hpp DESTINATION include)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	install(TARGETS CoreRenderRocket LIBRARY DESTINATION lib)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
