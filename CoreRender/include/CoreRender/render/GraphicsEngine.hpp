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

#include "VideoDriverType.hpp"
#include "RenderContext.hpp"
#include "../core/FileSystem.hpp"
#include "../core/Log.hpp"
#include "Texture2D.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "Pipeline.hpp"
#include "ShaderText.hpp"
#include "InputEvent.hpp"
#include "Model.hpp"

#include <queue>
#include "RenderStats.hpp"

namespace cr
{
namespace res
{
	class ResourceManager;
}
namespace render
{
	class RenderContext;
	class Renderer;
	class RenderThread;
	class VideoDriver;

	/**
	 * Main class of the engine providing the main intialization functions.
	 *
	 * This class provides code for initializing/shutting down the engine and
	 * contains pointers to the other main classes of the engine (mainly
	 * ResourceManager).
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
			 * Initializes the graphics engine.
			 *
			 * This first creates a render window if no context was specified
			 * by the user and then initializes the rendering and resource
			 * management system.
			 * @param type Type of the rendering backend.
			 * @param width Width of the render window (either the existing one
			 * if context was specified, or the one being created).
			 * @param height Height of the render window.
			 * @param fullscreen If a render window is created, this specifies
			 * whether the window is created in fullscreen or windowed mode.
			 * @param context Here you can specify an existing context for
			 * rendering. Use this if you e.g. want to render into an existing
			 * window.
			 * @param multithreaded If this is set to true, the engine tries to
			 * clone the render context and then proceeds in multithreaded
			 * rendering mode. If RenderContext::clone() fails, it just proceeds
			 * in single-threaded mode leaving a warning in the log.
			 * @return True if the engine was successfully set up. If not,
			 * appropriate log messages are written.
			 */
			bool init(VideoDriverType::List type = VideoDriverType::OpenGL,
			          unsigned int width = 1024,
			          unsigned int height = 768,
			          bool fullscreen = false,
			          RenderContext::Ptr context = 0,
			          bool multithreaded = true);
			/**
			 * Resizes the render window connected to the render context. All
			 * resources shall stay valid after the resizing.
			 *
			 * This does not always work. In case this function returns false,
			 * you have to shut the engine down completely, call init again and
			 * reload all your resources.
			 * @param width New width of the render window.
			 * @param height New height of the render window.
			 * @param fullscreen Mode (fullscreen/windowed) of the new render
			 * window.
			 * @return True if the window was successfully resized.
			 */
			bool resize(unsigned int width,
			            unsigned int height,
			            bool fullscreen);
			/**
			 * Stops rendering and shuts the engine down.
			 *
			 * @note If you call this, make sure you do not have any references
			 * to any resources left, especially set all SharedPointer instances
			 * to resources to 0 before calling this. If you do not, the engine
			 * might crash.
			 */
			void shutdown();

			/**
			 * Inserts a pipeline at the end of the pipeline list.
			 * @todo Functions for inserting pipelines at a certain position.
			 */
			void addPipeline(Pipeline::Ptr pipeline);
			/**
			 * Removes the pipeline from the pipeline list.
			 */
			void removePipeline(Pipeline::Ptr pipeline);
			/**
			 * Returns the complete pipeline list.
			 */
			std::vector<Pipeline::Ptr> getPipelines();
			/**
			 * Returns the number of pipelines currently used for rendering.
			 */
			unsigned int getPipelineCount();

			/**
			 * Begins a new frame. This function also uploads all newly created
			 * objects, so make sure you do not manipulate any resources after
			 * you have called this which you might use within this frame.
			 *
			 * After having called this function, you may call
			 * Pipeline::submit() to fill the render queue with batches.
			 * @return Returns true if the frame was successfully started.
			 */
			bool beginFrame();
			/**
			 * Ends a frame and renders the content of the render queue. This
			 * first waits for the render thread to finish the last frame, then
			 * passes the render queue to the render thread and signals it to
			 * start rendering the frame.
			 * @return Returns false if the frame could not be rendered.
			 */
			bool endFrame();

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
			 * Injects an input event into the engine. This is usually called
			 * by RenderContext::update().
			 * @param event Event to be inserted into the input queue.
			 * @note This function is thread-safe.
			 */
			void injectInput(const InputEvent &event);
			/**
			 * Fetches the oldest element from the input queue. This element is
			 * then dropped from the queue.
			 * @param event Pointer to an InputEvent which is filled with the
			 * oldest input event.
			 * @return False if the input queue was empty and no event has been
			 * returned.
			 * @note This function is thread-safe.
			 */
			bool getInput(InputEvent *event);

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
			const RenderStats &getRenderStats()
			{
				return stats;
			}
		private:
			RenderContext::Ptr createContext(VideoDriverType::List type,
			                                 unsigned int width,
			                                 unsigned int height,
			                                 bool fullscreen);
			VideoDriver *createDriver(VideoDriverType::List type);

			void createDefaultResources();
			void destroyDefaultResources();

			res::ResourceManager *rmgr;
			core::FileSystem::Ptr fs;
			core::Log::Ptr log;

			bool multithreaded;
			RenderContext::Ptr context;
			RenderContext::Ptr secondcontext;

			Renderer *renderer;
			VideoDriver *driver;
			RenderThread *renderthread;

			tbb::spin_mutex pipelinemutex;
			std::vector<Pipeline::Ptr> pipelines;

			tbb::spin_mutex inputmutex;
			std::queue<InputEvent> inputqueue;

			RenderStats stats;

			PipelineInfo *renderdata;

			// Default resources
			IndexBuffer::Ptr fsquadib;
			VertexBuffer::Ptr fsquadvb;
	};
}
}

#endif
