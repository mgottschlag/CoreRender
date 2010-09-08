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

#include "CoreRender/render/GraphicsEngine.hpp"
#include "CoreRender/core/StandardFileSystem.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/RenderContextNull.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/render/RenderThread.hpp"
#include "CoreRender/res/DefaultResourceFactory.hpp"
#include "CoreRender/render/Animation.hpp"
#include "FrameData.hpp"
#include "CoreRender/core/MemoryPool.hpp"

#if defined(CORERENDER_USE_SDL)
	#include "opengl/RenderContextSDL.hpp"
#elif defined(CORERENDER_USE_GLCML)
	#include "opengl/RenderContextGLCML.hpp"
#elif defined(CORERENDER_USE_GLFW)
	#include "opengl/RenderContextGLFW.hpp"
#endif
#include "opengl/VideoDriverOpenGL.hpp"
#include "null/VideoDriverNull.hpp"

namespace cr
{
namespace render
{
	class Texture2DFactory : public res::ResourceFactory
	{
		public:
			Texture2DFactory(render::VideoDriver *driver,
			                      render::Renderer *renderer,
			                      res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver), renderer(renderer)
			{
			}
			virtual ~Texture2DFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createTexture2D(renderer, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;
	};
	class VertexBufferFactory : public res::ResourceFactory
	{
		public:
			VertexBufferFactory(render::VideoDriver *driver,
			                      render::Renderer *renderer,
			                      res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver), renderer(renderer)
			{
			}
			virtual ~VertexBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createVertexBuffer(renderer, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;
	};
	class IndexBufferFactory : public res::ResourceFactory
	{
		public:
			IndexBufferFactory(render::VideoDriver *driver,
			                      render::Renderer *renderer,
			                      res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver), renderer(renderer)
			{
			}
			virtual ~IndexBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createIndexBuffer(renderer, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;
	};
	class ShaderFactory : public res::ResourceFactory
	{
		public:
			ShaderFactory(render::VideoDriver *driver,
			              render::Renderer *renderer,
			              res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver), renderer(renderer)
			{
			}
			virtual ~ShaderFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createShader(renderer, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;
	};
	class FrameBufferFactory : public res::ResourceFactory
	{
		public:
			FrameBufferFactory(render::VideoDriver *driver,
			                   render::Renderer *renderer,
			                   res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver), renderer(renderer)
			{
			}
			virtual ~FrameBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createFrameBuffer(renderer, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;
	};

	GraphicsEngine::GraphicsEngine()
		: rmgr(0), multithreaded(true), renderer(0), renderthread(0)
	{
	}
	GraphicsEngine::~GraphicsEngine()
	{
	}

	bool GraphicsEngine::init(VideoDriverType::List type,
	                          unsigned int width,
	                          unsigned int height,
	                          bool fullscreen,
	                          RenderContext::Ptr context,
	                          bool multithreaded)
	{
		// Initialize file system
		if (!fs)
		{
			core::StandardFileSystem::Ptr newfs = new core::StandardFileSystem();
			newfs->mount("", "/");
			fs = newfs;
		}
		// Initialize log file
		if (!log)
			log = new core::Log(fs, "/CoreRenderLog.html");
		log->info("CoreRender initializing.");
		// TODO: Version information
		// Create context of no was provided
		if (!context)
		{
			context = createContext(type, width, height, fullscreen);
		}
		if (!context)
		{
			log->error("Could not create any rendering context!");
			return false;
		}
		this->context = context;
		if (multithreaded)
		{
			// Create second context for the rendering thread
			RenderContext::Ptr secondcontext = context->clone();
			if (secondcontext)
			{
				this->secondcontext = secondcontext;
			}
			else
			{
				log->warning("Could not clone the rendering context. "
				             "Continuing single-threaded.");
				multithreaded = false;
			}
		}
		this->multithreaded = multithreaded;
		// Initialize resource manager
		rmgr = new res::ResourceManager(fs, log);
		if (!rmgr->init())
		{
			log->error("Could not initialize resource manager.");
			delete rmgr;
			rmgr = 0;
			context = 0;
			secondcontext = 0;
			return false;
		}
		// Create renderer
		context->makeCurrent();
		driver = createDriver(type);
		context->makeCurrent(false);
		if (!driver)
		{
			log->error("Could not create video driver.");
			delete rmgr;
			rmgr = 0;
			context = 0;
			secondcontext = 0;
			return false;
		}
		renderer = new Renderer(context, secondcontext, log, driver, this);
		// Create render thread
		if (multithreaded)
		{
			renderthread = new RenderThread(renderer);
			if (!renderthread->start())
			{
				log->error("Could not start render thread.");
				delete renderthread;
				renderthread = 0;
				delete renderer;
				renderer = 0;
				delete rmgr;
				rmgr = 0;
				context = 0;
				secondcontext = 0;
				return false;
			}
		}
		// Register resource types
		res::ResourceFactory::Ptr factory;
		factory = new res::DefaultResourceFactory<Material>(rmgr);
		rmgr->addFactory("Material", factory);
		factory = new res::DefaultResourceFactory<Model>(rmgr);
		rmgr->addFactory("Model", factory);
		factory = new res::DefaultResourceFactory<ShaderText>(rmgr);
		rmgr->addFactory("ShaderText", factory);
		factory = new ShaderFactory(driver, renderer, rmgr);
		rmgr->addFactory("Shader", factory);
		factory = new Texture2DFactory(driver, renderer, rmgr);
		rmgr->addFactory("Texture2D", factory);
		factory = new IndexBufferFactory(driver, renderer, rmgr);
		rmgr->addFactory("IndexBuffer", factory);
		factory = new VertexBufferFactory(driver, renderer, rmgr);
		rmgr->addFactory("VertexBuffer", factory);
		factory = new FrameBufferFactory(driver, renderer, rmgr);
		rmgr->addFactory("FrameBuffer", factory);
		factory = new res::DefaultResourceFactory<Animation>(rmgr);
		rmgr->addFactory("Animation", factory);
		factory = new res::DefaultResourceFactory<RenderTarget>(rmgr);
		rmgr->addFactory("RenderTarget", factory);
		return true;
	}
	bool GraphicsEngine::resize(unsigned int width,
	                            unsigned int height,
	                            bool fullscreen)
	{
		// TODO
		return false;
	}
	void GraphicsEngine::shutdown()
	{
		// Stop and destroy render thread
		if (multithreaded)
		{
			renderthread->stop();
			delete renderthread;
			renderthread = 0;
		}
		// Clean up render resources
		// TODO
		// Delete pipelines
		pipelines.clear();
		// Destroy renderer
		delete renderer;
		renderer = 0;
		// Delete driver
		delete driver;
		// Stop resource manager
		rmgr->shutdown();
		delete rmgr;
		rmgr = 0;
		// Close log
		log = 0;
		// Destroy file system
		fs = 0;
		// Delete contexts
		secondcontext = 0;
		context = 0;
	}

	bool GraphicsEngine::beginFrame()
	{
		renderer->uploadNewObjects();
		// Setup the rendering pipeline
		for (unsigned int i = 0; i < pipelines.size(); i++)
		{
			pipelines[i]->beginFrame();
		}
		return true;
	}
	bool GraphicsEngine::endFrame()
	{
		// Collect frame data
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		unsigned int memsize = sizeof(PipelineInfo) * pipelines.size();
		PipelineInfo *renderdata = (PipelineInfo*)memory->allocate(memsize);
		for (unsigned int i = 0; i < pipelines.size(); i++)
		{
			pipelines[i]->prepare(&renderdata[i]);
		}
		// Wait for last frame to end
		if (multithreaded)
			renderthread->waitForFrame();
		// Fetch statistics from the last frame
		stats = driver->getStats();
		driver->getStats().reset();
		// Render
		renderer->prepareRendering(renderdata, pipelines.size());
		if (multithreaded)
			renderthread->startFrame();
		else
		{
			driver->getStats().setFrameBegin(core::Time::Now());
			driver->getStats().setRenderBegin(core::Time::Now());
			renderer->render();
			driver->getStats().setFrameEnd(core::Time::Now());
		}
		// Update input in secondary context
		// SDL needs this.
		if (secondcontext)
			secondcontext->update(this);
		return true;
	}

	void GraphicsEngine::addPipeline(Pipeline::Ptr pipeline)
	{
		pipeline->setRenderer(renderer);
		// TODO: In debug mode, check whether we are currently rendering?
		tbb::spin_mutex::scoped_lock lock(pipelinemutex);
		pipelines.push_back(pipeline);
	}
	void GraphicsEngine::removePipeline(Pipeline::Ptr pipeline)
	{
		tbb::spin_mutex::scoped_lock lock(pipelinemutex);
		for (unsigned int i = 0; i < pipelines.size(); i++)
		{
			if (pipelines[i] == pipeline)
			{
				pipelines.erase(pipelines.begin() + i);
				return;
			}
		}
	}
	std::vector<Pipeline::Ptr> GraphicsEngine::getPipelines()
	{
		tbb::spin_mutex::scoped_lock lock(pipelinemutex);
		return pipelines;
	}
	unsigned int GraphicsEngine::getPipelineCount()
	{
		tbb::spin_mutex::scoped_lock lock(pipelinemutex);
		return pipelines.size();
	}

	void GraphicsEngine::setFileSystem(core::FileSystem::Ptr fs)
	{
		// TODO: Maybe not thread-safe
		this->fs = fs;
		if (rmgr)
		{
			rmgr->setFileSystem(fs);
		}
	}
	void GraphicsEngine::setLog(core::Log::Ptr log)
	{
		this->log = log;
	}

	void GraphicsEngine::injectInput(const InputEvent &event)
	{
		tbb::spin_mutex::scoped_lock lock(inputmutex);
		inputqueue.push(event);
	}
	bool GraphicsEngine::getInput(InputEvent *event)
	{
		tbb::spin_mutex::scoped_lock lock(inputmutex);
		if (inputqueue.empty())
			return false;
		*event = inputqueue.front();
		inputqueue.pop();
		return true;
	}

	RenderContext::Ptr GraphicsEngine::createContext(VideoDriverType::List type,
	                                                 unsigned int width,
	                                                 unsigned int height,
	                                                 bool fullscreen)
	{
		if (type == VideoDriverType::OpenGL)
		{
#if defined(CORERENDER_USE_SDL)
			opengl::RenderContextSDL *newctx;
			newctx = new opengl::RenderContextSDL();
			if (!newctx->create(width, height, fullscreen))
			{
				delete newctx;
				return 0;
			}
			return newctx;
#elif defined(CORERENDER_USE_GLCML)
			opengl::RenderContextGLCML *newctx;
			newctx = new opengl::RenderContextGLCML();
			if (!newctx->create(width, height, fullscreen))
			{
				delete newctx;
				return 0;
			}
			return newctx;
#elif defined(CORERENDER_USE_GLFW)
			opengl::RenderContextGLFW *newctx;
			newctx = new opengl::RenderContextGLFW();
			if (!newctx->create(width, height, fullscreen))
			{
				delete newctx;
				return 0;
			}
			return newctx;
#endif
		}
		else if (type == VideoDriverType::Null)
		{
			return new RenderContextNull();
		}
		else
		{
			return 0;
		}
	}
	VideoDriver *GraphicsEngine::createDriver(VideoDriverType::List type)
	{
		if (type == VideoDriverType::OpenGL)
		{
			opengl::VideoDriverOpenGL *driver;
			driver = new opengl::VideoDriverOpenGL(log);
			if (!driver->init())
			{
				delete driver;
				return 0;
			}
			return driver;
		}
		else if (type == VideoDriverType::Null)
		{
			return new null::VideoDriverNull();
		}
		else
		{
			return 0;
		}
	}
}
}
