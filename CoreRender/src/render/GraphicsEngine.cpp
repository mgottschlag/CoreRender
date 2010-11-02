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
#include "CoreRender/res/DefaultResourceFactory.hpp"
#include "CoreRender/render/Animation.hpp"
#include "CoreRender/render/RenderTarget.hpp"
#include "CoreRender/render/Model.hpp"
#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/Pipeline.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/core/MemoryPool.hpp"

#include "opengl/VideoDriverOpenGL.hpp"

namespace cr
{
namespace render
{
	class Texture2DFactory : public res::ResourceFactory
	{
		public:
			Texture2DFactory(render::VideoDriver *driver,
			                 UploadManager &uploadmgr,
			                 res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}
			virtual ~Texture2DFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createTexture2D(uploadmgr, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			UploadManager &uploadmgr;
	};
	class VertexBufferFactory : public res::ResourceFactory
	{
		public:
			VertexBufferFactory(render::VideoDriver *driver,
			                    UploadManager &uploadmgr,
			                    res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}
			virtual ~VertexBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createVertexBuffer(uploadmgr, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			UploadManager &uploadmgr;
	};
	class IndexBufferFactory : public res::ResourceFactory
	{
		public:
			IndexBufferFactory(render::VideoDriver *driver,
			                   UploadManager &uploadmgr,
			                   res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}
			virtual ~IndexBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createIndexBuffer(uploadmgr, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			UploadManager &uploadmgr;
	};
	class ShaderFactory : public res::ResourceFactory
	{
		public:
			ShaderFactory(render::VideoDriver *driver,
			              UploadManager &uploadmgr,
			              res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}
			virtual ~ShaderFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createShader(uploadmgr, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			UploadManager &uploadmgr;
	};
	class FrameBufferFactory : public res::ResourceFactory
	{
		public:
			FrameBufferFactory(render::VideoDriver *driver,
			                   UploadManager &uploadmgr,
			                   res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}
			virtual ~FrameBufferFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createFrameBuffer(uploadmgr, getManager(), name);
			}
		private:
			render::VideoDriver *driver;
			UploadManager &uploadmgr;
	};

	GraphicsEngine::GraphicsEngine()
		: rmgr(0), driver(0)
	{
	}
	GraphicsEngine::~GraphicsEngine()
	{
	}

	bool GraphicsEngine::init()
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
		// Initialize resource manager
		rmgr = new res::ResourceManager(fs, log);
		if (!rmgr->init())
		{
			log->error("Could not initialize resource manager.");
			delete rmgr;
			rmgr = 0;
			return false;
		}
		// Create video driver
		driver = createDriver();
		if (!driver)
		{
			log->error("Could not create video driver.");
			delete rmgr;
			rmgr = 0;
			return false;
		}
		uploadmgr.setCaps(&driver->getCaps());
		// Register resource types
		res::ResourceFactory::Ptr factory;
		factory = new res::DefaultResourceFactory<Material>(rmgr);
		rmgr->addFactory("Material", factory);
		factory = new res::DefaultResourceFactory<Model>(rmgr);
		rmgr->addFactory("Model", factory);
		factory = new ShaderFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("Shader", factory);
		factory = new Texture2DFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("Texture2D", factory);
		factory = new IndexBufferFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("IndexBuffer", factory);
		factory = new VertexBufferFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("VertexBuffer", factory);
		factory = new FrameBufferFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("FrameBuffer", factory);
		factory = new res::DefaultResourceFactory<Animation>(rmgr);
		rmgr->addFactory("Animation", factory);
		factory = new res::DefaultResourceFactory<RenderTarget>(rmgr);
		rmgr->addFactory("RenderTarget", factory);
		factory = new res::DefaultResourceFactory<Pipeline>(rmgr);
		rmgr->addFactory("Pipeline", factory);
		// Create default resources
		// TODO
		return true;
	}
	void GraphicsEngine::shutdown()
	{
		// Delete default resources
		// TODO
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
	}

	FrameData *GraphicsEngine::beginFrame()
	{
		// TODO: Reuse memory
		core::MemoryPool *memory = new core::MemoryPool;
		FrameData *frame = new FrameData(memory);
		return frame;
	}
	void GraphicsEngine::endFrame(FrameData *frame)
	{
		// TODO: Create lists with resources to be uploaded and deleted
		// TODO: Render stats
	}
	void GraphicsEngine::render(FrameData *frame)
	{
		// Upload resources which need to be uploaded
		// TODO
		// Render frame
		// TODO
		// Delete resources which need to be deleted
		// TODO
	}
	void GraphicsEngine::discard(FrameData *frame)
	{
		// Upload resources which need to be uploaded
		// TODO
		// Delete resources which need to be deleted
		// TODO
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

	VideoDriver *GraphicsEngine::createDriver()
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
}
}
