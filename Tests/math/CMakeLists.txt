
include_directories(../../CoreRender/include)

add_executable(Matrix4 Matrix4.cpp)
target_link_libraries(Matrix4 CoreRender)
