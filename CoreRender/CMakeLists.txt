
project(CoreRender)
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual")

set(SRC
	include/CoreRender.hpp
	include/CoreRender/core/Buffer.hpp
	include/CoreRender/core/Color.hpp
	include/CoreRender/core/ConditionVariable.hpp
	include/CoreRender/core/File.hpp
	include/CoreRender/core/FileList.hpp
	include/CoreRender/core/FileSystem.hpp
	include/CoreRender/core/FrameLimiter.hpp
	include/CoreRender/core/Functor.hpp
	include/CoreRender/core/Hardware.hpp
	include/CoreRender/core/Log.hpp
	include/CoreRender/core/MemoryPool.hpp
	include/CoreRender/core/ReferenceCounted.hpp
	include/CoreRender/core/Semaphore.hpp
	include/CoreRender/core/StandardFile.hpp
	include/CoreRender/core/StandardFileSystem.hpp
	include/CoreRender/core/Thread.hpp
	include/CoreRender/core/Time.hpp
	include/CoreRender/math/Vector3.hpp
	include/CoreRender/math/Alignment.hpp
	include/CoreRender/math/Quaternion.hpp
	include/CoreRender/math/Matrix4.hpp
	include/CoreRender/math/Math.hpp
	include/CoreRender/math/Vector2.hpp
	include/CoreRender/math/ScreenPosition.hpp
	include/CoreRender/math/StdInt.hpp
	include/CoreRender/res/LoadingThread.hpp
	include/CoreRender/res/Resource.hpp
	include/CoreRender/res/ResourceManager.hpp
	include/CoreRender/render/RenderBatch.hpp
	include/CoreRender/render/RenderResource.hpp
	include/CoreRender/render/RenderThread.hpp
	include/CoreRender/render/RenderStats.hpp
	include/CoreRender/render/RenderCaps.hpp
	include/CoreRender/render/Shader.hpp
	include/CoreRender/render/VideoDriverType.hpp
	include/CoreRender/render/RenderContextOpenGL.hpp
	include/CoreRender/render/RenderContextReuseOpenGL.hpp
	include/CoreRender/render/GeometryManager.hpp
	include/CoreRender/render/RenderPass.hpp
	include/CoreRender/render/Texture.hpp
	include/CoreRender/render/RenderContext.hpp
	include/CoreRender/render/Material.hpp
	include/CoreRender/render/RenderTarget.hpp
	include/CoreRender/render/GraphicsEngine.hpp
	src/core/Buffer.cpp
	src/core/ConditionVariable.cpp
	src/core/FrameLimiter.cpp
	src/core/Log.cpp
	src/core/Semaphore.cpp
	src/core/StandardFile.cpp
	src/core/StandardFileSystem.cpp
	src/core/Thread.cpp
	src/core/Time.cpp
	src/render/GraphicsEngine.cpp
	src/render/null/VideoDriverNull.hpp
	src/render/opengl/RenderContextGLCML.cpp
	src/render/opengl/RenderContextGLCML.hpp
	src/render/opengl/RenderWindowGLCML.cpp
	src/render/opengl/RenderWindowGLCML.hpp
	src/render/opengl/VideoDriverOpenGL.cpp
	src/render/opengl/VideoDriverOpenGL.hpp
	src/render/RenderContextReuseOpenGL.cpp
	src/render/Renderer.cpp
	src/render/RenderResource.cpp
	src/render/RenderThread.cpp
	src/res/LoadingThread.cpp
	src/res/Resource.cpp
	src/res/ResourceManager.cpp
)

include_directories(include)

add_library(CoreRender SHARED ${SRC})
target_link_libraries(CoreRender tbb pthread GL freeimageplus glcml)

install(DIRECTORY include/CoreRender DESTINATION include/CoreRender FILES_MATCHING PATTERN *.hpp)
install(FILES include/CoreRender.hpp DESTINATION include)
install(TARGETS CoreRender LIBRARY DESTINATION lib)

