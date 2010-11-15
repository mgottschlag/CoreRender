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

#include "CoreRender/GraphicsEngine.hpp"
#include "CoreRender/core/StandardFileSystem.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/res/DefaultResourceFactory.hpp"
#include "CoreRender/render/RenderTarget.hpp"
#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/Pipeline.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/render/Material.hpp"
#include "CoreRender/core/MemoryPool.hpp"

#include "render/opengl/VideoDriverOpenGL.hpp"

namespace cr
{
	class Texture2DFactory : public res::ResourceFactory
	{
		public:
			Texture2DFactory(render::VideoDriver *driver,
			                 render::UploadManager &uploadmgr,
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
			render::UploadManager &uploadmgr;
	};
	class VertexBufferFactory : public res::ResourceFactory
	{
		public:
			VertexBufferFactory(render::VideoDriver *driver,
			                    render::UploadManager &uploadmgr,
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
			render::UploadManager &uploadmgr;
	};
	class IndexBufferFactory : public res::ResourceFactory
	{
		public:
			IndexBufferFactory(render::VideoDriver *driver,
			                   render::UploadManager &uploadmgr,
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
			render::UploadManager &uploadmgr;
	};
	class ShaderFactory : public res::ResourceFactory
	{
		public:
			ShaderFactory(render::VideoDriver *driver,
			              render::UploadManager &uploadmgr,
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
			render::UploadManager &uploadmgr;
	};
	class FrameBufferFactory : public res::ResourceFactory
	{
		public:
			FrameBufferFactory(render::VideoDriver *driver,
			                   render::UploadManager &uploadmgr,
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
			render::UploadManager &uploadmgr;
	};
	class MaterialFactory : public res::ResourceFactory
	{
		public:
			MaterialFactory(render::UploadManager &uploadmgr,
			                res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), uploadmgr(uploadmgr)
			{
			}
			virtual ~MaterialFactory()
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return new render::Material(uploadmgr, getManager(), name);
			}
		private:
			render::UploadManager &uploadmgr;
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
		factory = new MaterialFactory(uploadmgr, rmgr);
		rmgr->addFactory("Material", factory);
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
		factory = new res::DefaultResourceFactory<render::RenderTarget>(rmgr);
		rmgr->addFactory("RenderTarget", factory);
		factory = new res::DefaultResourceFactory<render::Pipeline>(rmgr);
		rmgr->addFactory("Pipeline", factory);
		factory = new res::DefaultResourceFactory<scene::Mesh>(rmgr);
		rmgr->addFactory("Mesh", factory);
		factory = new res::DefaultResourceFactory<scene::Animation>(rmgr);
		rmgr->addFactory("Animation", factory);
		// TODO: Scene resources
		// Create default resources
		// TODO
		return true;
	}
	void GraphicsEngine::shutdown()
	{
		// Delete default resources
		// TODO
		// Delete remaining resources
		// This has to be done repeatedly as a resource can hold references to
		// other resources which can only be freed after the resource is deleted
		unsigned deleted = 0;
		do
		{
			core::MemoryPool deletionmem;
			render::UploadLists deletionlists;
			uploadmgr.getLists(deletionlists, &deletionmem);
			// We have to call uploadResources() to prevent memory leaks here as
			// the upload data is deleted in the upload functions
			uploadmgr.uploadResources(deletionlists);
			uploadmgr.deleteResources(deletionlists);
			deleted = deletionlists.objdeletioncount + deletionlists.resdeletioncount;
		}
		while (deleted > 0);
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

	render::FrameData *GraphicsEngine::beginFrame()
	{
		// TODO: Reuse memory
		core::MemoryPool *memory = new core::MemoryPool;
		render::FrameData *frame = new render::FrameData(memory);
		return frame;
	}
	void GraphicsEngine::endFrame(render::FrameData *frame)
	{
		// Create lists with resources to be uploaded and deleted
		uploadmgr.getLists(frame->getUploadLists(), frame->getMemory());
		// TODO: Render stats
	}
	void GraphicsEngine::render(render::FrameData *frame)
	{
		// Upload resources which need to be uploaded
		uploadmgr.uploadResources(frame->getUploadLists());
		// Render frame
		const std::vector<render::SceneFrameData*> &scenes = frame->getScenes();
		for (unsigned int i = 0; i < scenes.size(); i++)
		{
			driver->executeCommands(scenes[i]->getFirstCommand());
		}
		// Delete resources which need to be deleted
		uploadmgr.deleteResources(frame->getUploadLists());
		// Delete frame data
		core::MemoryPool *memory = frame->getMemory();
		delete frame;
		delete memory;
	}
	void GraphicsEngine::discard(render::FrameData *frame)
	{
		// Upload resources which need to be uploaded
		uploadmgr.uploadResources(frame->getUploadLists());
		// Delete resources which need to be deleted
		uploadmgr.deleteResources(frame->getUploadLists());
		// Delete frame data
		core::MemoryPool *memory = frame->getMemory();
		delete frame;
		delete memory;
	}

	scene::Mesh::Ptr GraphicsEngine::getMesh(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Mesh", name);
		return scene::Mesh::Ptr((scene::Mesh*)res.get());
	}
	scene::Animation::Ptr GraphicsEngine::getAnimation(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Animation", name);
		return scene::Animation::Ptr((scene::Animation*)res.get());
	}
	render::Pipeline::Ptr GraphicsEngine::getPipeline(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Pipeline", name);
		return render::Pipeline::Ptr((render::Pipeline*)res.get());
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

	render::VideoDriver *GraphicsEngine::createDriver()
	{
		render::opengl::VideoDriverOpenGL *driver;
		driver = new render::opengl::VideoDriverOpenGL(log);
		if (!driver->init())
		{
			delete driver;
			return 0;
		}
		return driver;
	}
}
