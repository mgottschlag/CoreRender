
include_directories(../../CoreRender/include)

add_executable(MemoryPool MemoryPool.cpp)
target_link_libraries(MemoryPool CoreRender)
