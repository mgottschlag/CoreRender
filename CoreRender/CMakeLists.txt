
project(CoreRender)
cmake_minimum_required(VERSION 2.6.0)

# Main CoreRender library

set(SRC
	include/CoreRender/core/Color.hpp
	include/CoreRender/core/File.hpp
	include/CoreRender/core/FileList.hpp
	include/CoreRender/core/FileSystem.hpp
	include/CoreRender/core/Functor.hpp
	include/CoreRender/core/Hardware.hpp
	include/CoreRender/core/HashMap.hpp
	include/CoreRender/core/Log.hpp
	include/CoreRender/core/MemoryPool.hpp
	include/CoreRender/core/Platform.hpp
	include/CoreRender/core/ReferenceCounted.hpp
	include/CoreRender/core/Semaphore.hpp
	include/CoreRender/core/StandardFile.hpp
	include/CoreRender/core/StandardFileSystem.hpp
	include/CoreRender/core/StructPacking.hpp
	include/CoreRender/core/Thread.hpp
	include/CoreRender/core/Time.hpp
	include/CoreRender.hpp
	include/CoreRender/math/Alignment.hpp
	include/CoreRender/math/Math.hpp
	include/CoreRender/math/Matrix3.hpp
	include/CoreRender/math/Matrix4.hpp
	include/CoreRender/math/Quaternion.hpp
	include/CoreRender/math/ScreenPosition.hpp
	include/CoreRender/math/StdInt.hpp
	include/CoreRender/math/Vector2.hpp
	include/CoreRender/math/Vector3.hpp
	include/CoreRender/math/Vector4.hpp
	include/CoreRender/render/BlendMode.hpp
	include/CoreRender/render/FrameBuffer.hpp
	include/CoreRender/render/FrameData.hpp
	include/CoreRender/render/GeometryManager.hpp
	include/CoreRender/GraphicsEngine.hpp
	include/CoreRender/render/IndexBuffer.hpp
	include/CoreRender/render/Material.hpp
	include/CoreRender/render/OcclusionQuery.hpp
	include/CoreRender/render/Pipeline.hpp
	include/CoreRender/render/RenderCaps.hpp
	include/CoreRender/render/RenderQuery.hpp
	include/CoreRender/render/RenderResource.hpp
	include/CoreRender/render/RenderStats.hpp
	include/CoreRender/render/RenderTarget.hpp
	include/CoreRender/render/RenderObject.hpp
	include/CoreRender/render/ShaderCombination.hpp
	include/CoreRender/render/Shader.hpp
	include/CoreRender/render/ShaderVariableType.hpp
	include/CoreRender/render/Texture.hpp
	include/CoreRender/render/TimerQuery.hpp
	include/CoreRender/render/VertexBuffer.hpp
	include/CoreRender/render/VertexLayout.hpp
	include/CoreRender/render/VideoDriverType.hpp
	include/CoreRender/res/DefaultResourceFactory.hpp
	include/CoreRender/res/LoadingThread.hpp
	include/CoreRender/res/ResourceFactory.hpp
	include/CoreRender/res/Resource.hpp
	include/CoreRender/res/ResourceManager.hpp
	include/CoreRender/scene/Animation.hpp
	src/3rdparty/tinystr.cpp
	src/3rdparty/tinystr.h
	src/3rdparty/tinyxml.cpp
	src/3rdparty/tinyxml.h
	src/3rdparty/tinyxmlerror.cpp
	src/3rdparty/tinyxmlparser.cpp
	src/3rdparty/stb_image.h
	src/3rdparty/stb_image.c
	src/core/Log.cpp
	src/core/MemoryPool.cpp
	src/core/Semaphore.cpp
	src/core/StandardFile.cpp
	src/core/StandardFileSystem.cpp
	src/core/Thread.cpp
	src/core/Time.cpp
	src/render/FrameBuffer.cpp
	src/GraphicsEngine.cpp
	src/render/IndexBuffer.cpp
	src/render/Material.cpp
	src/render/opengl/FrameBufferOpenGL.cpp
	src/render/opengl/IndexBufferOpenGL.cpp
	src/render/opengl/RenderCapsOpenGL.cpp
	src/render/opengl/ShaderOpenGL.cpp
	src/render/opengl/TextureOpenGL.cpp
	src/render/opengl/VertexBufferOpenGL.cpp
	src/render/opengl/VideoDriverOpenGL.cpp
	src/render/Pipeline.cpp
	src/render/RenderResource.cpp
	src/render/RenderObject.cpp
	src/render/RenderTarget.cpp
	src/render/Shader.cpp
	src/render/ShaderCombination.cpp
	src/render/Texture.cpp
	src/render/UploadManager.cpp
	src/render/VertexBuffer.cpp
	src/render/VideoDriver.cpp
	src/render/VideoDriver.hpp
	src/res/LoadingThread.cpp
	src/res/Resource.cpp
	src/res/ResourceManager.cpp
	src/scene/Scene.cpp
	src/scene/AnimatedMesh.cpp
	src/scene/Animation.cpp
	src/scene/Model.cpp
	src/scene/Camera.cpp
	src/scene/Animation.cpp
	src/scene/AnimationBinding.cpp
	src/scene/PointLight.cpp
	src/scene/SpotLight.cpp
	src/scene/Light.cpp
	src/render/ImageSTB.cpp
	src/render/ImageDDS.cpp
)

set(LIB
	tbb
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(LIB
		${LIB}
		OpenGL32
		ws2_32
		glew32
		glu32
	)
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual")

	set(LIB
		${LIB}
		pthread
		GL
		GLEW
	)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(include)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRender STATIC ${SRC})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRender SHARED ${SRC})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(CoreRender ${LIB})

# Installation of files

install(DIRECTORY include/CoreRender DESTINATION include FILES_MATCHING PATTERN *.hpp)
install(FILES include/CoreRender.hpp DESTINATION include)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	install(TARGETS CoreRender LIBRARY DESTINATION lib)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
