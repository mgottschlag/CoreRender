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

#if defined(CORERENDER_USE_GLCML)
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
	GraphicsEngine::GraphicsEngine()
		: multithreaded(true), renderer(0), renderthread(0)
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
			newfs->mount("", "");
			fs = newfs.get();
		}
		// Initialize log file
		if (!log)
			log = new core::Log(fs, "CoreRenderLog.html");
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
				this->multithreaded = false;
			}
		}
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
		renderer = new Renderer(context, secondcontext, log);
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
		return true;
	}
	bool GraphicsEngine::resize(unsigned int width,
	                            unsigned int height,
	                            bool fullscreen)
	{
		// TODO
		return false;
	}
	bool GraphicsEngine::shutdown()
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
		// Destroy renderer
		delete renderer;
		renderer = 0;
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
		return true;
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

	void GraphicsEngine::injectInput(InputEvent *event)
	{
	}
	InputEvent *GraphicsEngine::getInput()
	{
		// TODO
		return 0;
	}

	RenderContext::Ptr GraphicsEngine::createContext(VideoDriverType::List type,
	                                                 unsigned int width,
	                                                 unsigned int height,
	                                                 bool fullscreen)
	{
		if (type == VideoDriverType::OpenGL)
		{
#if defined(CORERENDER_USE_GLCML)
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
			driver = new opengl::VideoDriverOpenGL();
			if (!driver->init())
			{
				delete driver;
				return 0;
			}
			return driver;
		}
		else if (type == VideoDriverType::OpenGL)
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
