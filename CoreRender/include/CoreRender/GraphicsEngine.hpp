/*
Copyright (C) 2010, Mathias Gottschlag

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
the Software, and to permit persons to whom the Software is furnished to do so,
subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef _CORERENDER_RENDER_GRAPHICSENGINE_HPP_INCLUDED_
#define _CORERENDER_RENDER_GRAPHICSENGINE_HPP_INCLUDED_

#include "core/FileSystem.hpp"
#include "core/Log.hpp"
#include "render/Pipeline.hpp"
#include "render/RenderStats.hpp"
#include "render/UploadManager.hpp"
#include "scene/Model.hpp"
#include "scene/Animation.hpp"

namespace cr
{
	namespace res
	{
		class ResourceManager;
	}
	namespace render
	{
		class VideoDriver;
		class FrameData;
	}

	/**
	 * Main class of the engine providing the main intialization functions.
	 *
	 * This class provides code for initializing/shutting down the engine and
	 * contains pointers to the other main classes of the engine (mainly
	 * ResourceManager). Also this class contains the main functions for
	 * rendering anything: beginFrame(), endFrame(), render().
	 *
	 * If you want to use CoreRender, the first thing to do is to create an
	 * instance of this class and then call init() (after having called
	 * setFileSystem() or setLog() if necessary).
	 */
	class GraphicsEngine
	{
		public:
			/**
			 * Constructor.
			 */
			GraphicsEngine();
			/**
			 * Destructor.
			 */
			~GraphicsEngine();

			/**
			 * Initializes the graphics engine, especially the rendering
			 * backend. The OpenGL context used for rendering has to be current.
			 * @return True if the engine was successfully set up. If not,
			 * appropriate log messages are written.
			 * @todo Additional information for the video driver? OpenGL
			 * version? Maybe even DirectX context information?
			 */
			bool init();
			/**
			 * Stops rendering and shuts the engine down. The OpenGL context
			 * used for rendering has to be current.
			 * @note If you call this, make sure you do not have any references
			 * to any resources left, especially set all SharedPointer instances
			 * to resources to 0 before calling this. If you do not, the engine
			 * might crash.
			 */
			void shutdown();

			render::FrameData *beginFrame();
			void endFrame(render::FrameData *frame);
			void render(render::FrameData *frame);
			void discard(render::FrameData *frame);

			scene::Model::Ptr getModel(const std::string name);
			scene::Animation::Ptr getAnimation(const std::string name);
			render::Pipeline::Ptr getPipeline(const std::string name);
			render::Material::Ptr getMaterial(const std::string name);

			/**
			 * Sets a user-specified file system for the engine. This can be
			 * called before calling init().
			 * @note The file system is reset at shutdown(), so you might have
			 * to call this again before calling init() again.
			 * @param fs New user-specified file system.
			 */
			void setFileSystem(core::FileSystem::Ptr fs);
			/**
			 * Returns the file system which is currently in use in the engine.
			 * This can either be the one specified via setFileSystem() of the
			 * one implicitly created in init() if none was specified.
			 * @note This function is thread-safe.
			 */
			core::FileSystem::Ptr getFileSystem()
			{
				return fs;
			}
			/**
			 * Sets a user-specified log writer. This can be called before
			 * calling init(), if not, the engine creates a default log writer.
			 * @param log User-specified log writer.
			 */
			void setLog(core::Log::Ptr log);
			/**
			 * Returns the log writer the engine currently uses.
			 * @note This function is thread-safe.
			 */
			core::Log::Ptr getLog()
			{
				return log;
			}

			/**
			 * Returns the resource manager used by the engine. The resource
			 * manager is created in init() and deleted in shutdown().
			 * @return Resource manager created by the engine.
			 */
			res::ResourceManager *getResourceManager()
			{
				return rmgr;
			}
			/**
			 * Returns the rendering statistics from the last completely
			 * finished frame. Note that this frame is not the one from the last
			 * endFrame() but rather the one before that if multithreading is
			 * enabled.
			 * @return Render statistics.
			 */
			const render::RenderStats &getRenderStats()
			{
				return stats;
			}
		private:
			render::VideoDriver *createDriver();

			render::UploadManager uploadmgr;

			res::ResourceManager *rmgr;
			core::FileSystem::Ptr fs;
			core::Log::Ptr log;

			render::VideoDriver *driver;

			render::RenderStats stats;
	};
}

#endif
