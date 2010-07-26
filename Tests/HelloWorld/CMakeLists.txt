
set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter")

include_directories(../../CoreRender/include)

add_executable(HelloWorld HelloWorld.cpp)
target_link_libraries(HelloWorld CoreRender glfw)
