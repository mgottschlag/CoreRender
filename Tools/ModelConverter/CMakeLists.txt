
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual")
	include_directories(/usr/local/include/assimp)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(../../CoreRender/src/3rdparty ../../CoreRender/include)

set(SRC
	src/main.cpp
	../../CoreRender/src/3rdparty/tinystr.cpp
	../../CoreRender/src/3rdparty/tinyxml.cpp
	../../CoreRender/src/3rdparty/tinyxmlerror.cpp
	../../CoreRender/src/3rdparty/tinyxmlparser.cpp
)

add_executable(ModelConverter ${SRC})
target_link_libraries(ModelConverter assimp)
