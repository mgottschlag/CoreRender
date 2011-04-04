
project(CoreRender)
cmake_minimum_required(VERSION 2.6.0)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../CMake/")

find_package(OpenGL REQUIRED)
find_package(TBB REQUIRED)
find_package(GameMath REQUIRED)

if(APPLE)
	include(FindOpenGL)
	find_library(COCOA_LIB Cocoa)
	find_library(GLFW_LIB GLFW)
	set(MAC_LIBRARIES ${GLFW_LIB} ${COCOA_LIB})
endif(APPLE)

# Main CoreRender library

set(INCLUDE
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
	include/CoreRender/core/StringTable.hpp
	include/CoreRender/core/StructPacking.hpp
	include/CoreRender/core/Thread.hpp
	include/CoreRender/core/Time.hpp
	include/CoreRender/GraphicsEngine.hpp
	include/CoreRender.hpp
	include/CoreRender/render/BlendMode.hpp
	include/CoreRender/render/DepthTest.hpp
	include/CoreRender/render/FrameBuffer.hpp
	include/CoreRender/render/FrameData.hpp
	include/CoreRender/render/GeometryManager.hpp
	include/CoreRender/render/IndexBuffer.hpp
	include/CoreRender/render/Material.hpp
	include/CoreRender/render/Mesh.hpp
	include/CoreRender/render/OcclusionQuery.hpp
	include/CoreRender/render/Pipeline.hpp
	include/CoreRender/render/PipelineStage.hpp
	include/CoreRender/render/PrimitiveType.hpp
	include/CoreRender/render/RenderCaps.hpp
	include/CoreRender/render/RenderObject.hpp
	include/CoreRender/render/RenderQuery.hpp
	include/CoreRender/render/RenderResource.hpp
	include/CoreRender/render/RenderStats.hpp
	include/CoreRender/render/RenderTarget.hpp
	include/CoreRender/render/ShaderCombination.hpp
	include/CoreRender/render/Shader.hpp
	include/CoreRender/render/ShaderVariableType.hpp
	include/CoreRender/render/Texture.hpp
	include/CoreRender/render/TimerQuery.hpp
	include/CoreRender/render/UploadManager.hpp
	include/CoreRender/render/VertexBuffer.hpp
	include/CoreRender/render/VertexLayout.hpp
	include/CoreRender/render/VideoDriverType.hpp
	include/CoreRender/res/DefaultResourceFactory.hpp
	include/CoreRender/res/LoadingThread.hpp
	include/CoreRender/res/NameRegistry.hpp
	include/CoreRender/res/ResourceFactory.hpp
	include/CoreRender/res/Resource.hpp
	include/CoreRender/res/ResourceManager.hpp
	include/CoreRender/scene/AnimatedModel.hpp
	include/CoreRender/scene/AnimationBinding.hpp
	include/CoreRender/scene/AnimationFile.hpp
	include/CoreRender/scene/Animation.hpp
	include/CoreRender/scene/Camera.hpp
	include/CoreRender/scene/GeometryFile.hpp
	include/CoreRender/scene/HeightMap.hpp
	include/CoreRender/scene/Light.hpp
	include/CoreRender/scene/Model.hpp
	include/CoreRender/scene/PointLight.hpp
	include/CoreRender/scene/Scene.hpp
	include/CoreRender/scene/SpotLight.hpp
	include/CoreRender/scene/Terrain.hpp
)

set(SRC
	${INCLUDE}
	src/3rdparty/stb_image.c
	src/3rdparty/tinystr.cpp
	src/3rdparty/tinyxml.cpp
	src/3rdparty/tinyxmlerror.cpp
	src/3rdparty/tinyxmlparser.cpp
	src/core/Log.cpp
	src/core/MemoryPool.cpp
	src/core/Semaphore.cpp
	src/core/StandardFile.cpp
	src/core/StandardFileSystem.cpp
	src/core/Thread.cpp
	src/core/Time.cpp
	src/GraphicsEngine.cpp
	src/render/FrameBuffer.cpp
	src/render/Image.cpp
	src/render/ImageLoaderDDS.cpp
	src/render/ImageLoaderSTB.cpp
	src/render/IndexBuffer.cpp
	src/render/Material.cpp
	src/render/Mesh.cpp
	src/render/opengl/FrameBufferOpenGL.cpp
	src/render/opengl/IndexBufferOpenGL.cpp
	src/render/opengl/MeshOpenGL.cpp
	src/render/opengl/RenderCapsOpenGL.cpp
	src/render/opengl/ShaderOpenGL.cpp
	src/render/opengl/TextureOpenGL.cpp
	src/render/opengl/VertexBufferOpenGL.cpp
	src/render/opengl/VideoDriverOpenGL.cpp
	src/render/Pipeline.cpp
	src/render/RenderObject.cpp
	src/render/RenderResource.cpp
	src/render/RenderTarget.cpp
	src/render/ShaderCombination.cpp
	src/render/Shader.cpp
	src/render/Texture.cpp
	src/render/UploadManager.cpp
	src/render/VertexBuffer.cpp
	src/render/VideoDriver.cpp
	src/res/LoadingThread.cpp
	src/res/Resource.cpp
	src/res/ResourceManager.cpp
	src/scene/AnimatedModel.cpp
	src/scene/AnimationBinding.cpp
	src/scene/Animation.cpp
	src/scene/Camera.cpp
	src/scene/HeightMap.cpp
	src/scene/Light.cpp
	src/scene/Model.cpp
	src/scene/PointLight.cpp
	src/scene/Scene.cpp
	src/scene/SpotLight.cpp
	src/scene/Terrain.cpp
)

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(LIB
		ws2_32
		glew32
	)
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	set(CMAKE_CXX_FLAGS "-Wall -Wextra -Wno-unused-parameter -Woverloaded-virtual")

	set(LIB
		pthread
		GLEW
	)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(include ${TBB_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${GAMEMATH_INCLUDE_DIR})

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRender STATIC ${SRC})
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	add_library(CoreRender SHARED ${SRC})
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

target_link_libraries(CoreRender ${LIB} ${OPENGL_LIBRARIES} ${MAC_LIBRARIES} ${TBB_LIBRARIES} ${GAMEMATH_LIBRARIES})

if(APPLE)
	set_property(PROPERTY FRAMEWORK)
	set_property(TARGET CoreRender PROPERTY PUBLIC_HEADERS ${INCLUDE})
endif(APPLE)

# Installation of files

install(DIRECTORY include/CoreRender DESTINATION include FILES_MATCHING PATTERN *.hpp)
install(FILES include/CoreRender.hpp DESTINATION include)
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
	install(TARGETS CoreRender LIBRARY DESTINATION lib)
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
