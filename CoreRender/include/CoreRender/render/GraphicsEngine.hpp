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

	class GraphicsEngine
	{
		public:
			GraphicsEngine();
			~GraphicsEngine();

			bool init(VideoDriverType::List type = VideoDriverType::OpenGL,
			          unsigned int width = 1024,
			          unsigned int height = 768,
			          bool fullscreen = false,
			          RenderContext::Ptr context = 0,
			          bool multithreaded = true);
			bool resize(unsigned int width,
			            unsigned int height,
			            bool fullscreen);
			bool shutdown();

			/**
			 * @todo Functions for inserting pipelines at a certain position.
			 */
			void addPipeline(Pipeline::Ptr pipeline);
			void removePipeline(Pipeline::Ptr pipeline);
			std::vector<Pipeline::Ptr> getPipelines();
			unsigned int getPipelineCount();

			bool beginFrame();
			bool endFrame();

			void setFileSystem(core::FileSystem::Ptr fs);
			core::FileSystem::Ptr getFileSystem()
			{
				return fs;
			}
			void setLog(core::Log::Ptr log);
			core::Log::Ptr getLog()
			{
				return log;
			}

			void injectInput(const InputEvent &event);
			bool getInput(InputEvent *event);

			res::ResourceManager *getResourceManager()
			{
				return rmgr;
			}
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
	};
}
}

#endif
