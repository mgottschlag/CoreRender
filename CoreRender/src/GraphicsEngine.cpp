/*
Copyright (C) 2011, Mathias Gottschlag

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
	class TextureFactory : public res::ResourceFactory
	{
		public:
			TextureFactory(render::VideoDriver *driver,
			               render::UploadManager &uploadmgr,
			               res::ResourceManager *rmgr)
				: res::ResourceFactory(rmgr), driver(driver),
				uploadmgr(uploadmgr)
			{
			}

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return driver->createTexture(uploadmgr, getManager(), name);
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

			virtual res::Resource::Ptr create(const std::string &name)
			{
				return new render::Material(uploadmgr, getManager(), name);
			}
		private:
			render::UploadManager &uploadmgr;
	};
	class ModelFactory : public res::ResourceFactory
	{
	public:
		ModelFactory(GraphicsEngine *graphics)
			: res::ResourceFactory(graphics->getResourceManager()),
			graphics(graphics)
		{
		}

		virtual res::Resource::Ptr create(const std::string &name)
		{
			return new scene::Model(graphics, name);
		}
	private:
		GraphicsEngine *graphics;
	};
	class TerrainFactory : public res::ResourceFactory
	{
	public:
		TerrainFactory(GraphicsEngine *graphics)
			: res::ResourceFactory(graphics->getResourceManager()),
			graphics(graphics)
		{
		}

		virtual res::Resource::Ptr create(const std::string &name)
		{
			return new scene::Terrain(graphics, name);
		}
	private:
		GraphicsEngine *graphics;
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
		rmgr = new res::ResourceManager(uploadmgr, fs, log);
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
		factory = new TextureFactory(driver, uploadmgr, rmgr);
		rmgr->addFactory("Texture", factory);
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
		factory = new ModelFactory(this);
		rmgr->addFactory("Model", factory);
		factory = new res::DefaultResourceFactory<scene::Animation>(rmgr);
		rmgr->addFactory("Animation", factory);
		factory = new TerrainFactory(this);
		rmgr->addFactory("Terrain", factory);
		// Create default resources
		// TODO
		// Statistics
		lastframeendtime = core::Time::Now();
		accumulationstart = lastframeendtime;
		lastaccumulationstart = accumulationstart;
		frames = 0;
		lastframes = 0;
		frametime = core::Duration::Nanoseconds(0);
		lastframetime = core::Duration::Nanoseconds(0);
		uploadtime = core::Duration::Nanoseconds(0);
		lastuploadtime = core::Duration::Nanoseconds(0);
		rendertime = core::Duration::Nanoseconds(0);
		lastrendertime = core::Duration::Nanoseconds(0);
		composetime = core::Duration::Nanoseconds(0);
		lastcomposetime = core::Duration::Nanoseconds(0);
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
		frame->endFrame();
	}
	void GraphicsEngine::render(render::FrameData *frame)
	{
		// Upload resources which need to be uploaded
		core::Time uploadstart = core::Time::Now();
		uploadmgr.uploadResources(frame->getUploadLists());
		core::Time uploadend = core::Time::Now();
		// Render frame
		core::Time renderstart = uploadend;
		driver->beginFrame();
		const std::vector<render::SceneFrameData*> &scenes = frame->getScenes();
		for (unsigned int i = 0; i < scenes.size(); i++)
		{
			driver->executeCommands(scenes[i]->getFirstCommand());
		}
		driver->endFrame();
		// Delete resources which need to be deleted
		uploadmgr.deleteResources(frame->getUploadLists());
		// Delete frame data
		core::MemoryPool *memory = frame->getMemory();
		core::Time composestart = frame->getComposeStartTime();
		core::Time composeend = frame->getComposeEndTime();
		delete frame;
		delete memory;
		core::Time renderend = core::Time::Now();
		// Statistics
		core::Duration timesincelastframe = renderend - lastframeendtime;
		lastframeendtime = renderend;
		render::RenderStats stats = driver->getStats();
		stats.fps = 1.0e9 / (double)timesincelastframe.getNanoseconds();
		stats.composetime = composeend - composestart;
		stats.uploadtime = uploadend - uploadstart;
		stats.rendertime = renderend - renderstart;
		stats.frametime = stats.composetime + stats.uploadtime + stats.rendertime;
		// Compute average values (accumulated over 1-2 seconds)
		frames++;
		unsigned int accumframes = frames + lastframes;
		composetime += stats.composetime;
		stats.averagecomposetime = (composetime + lastcomposetime) / accumframes;
		uploadtime += stats.uploadtime;
		stats.averageuploadtime = (uploadtime + lastuploadtime) / accumframes;
		rendertime += stats.rendertime;
		stats.averagerendertime = (rendertime + lastrendertime) / accumframes;
		frametime += stats.frametime;
		stats.averageframetime = (frametime + lastframetime) / accumframes;
		float accumseconds = 1.0e-9 * (renderend - lastaccumulationstart).getNanoseconds();
		stats.averagefps = (double)(frames + lastframes) / accumseconds;
		// Start a new accumulation frame
		if (renderend - accumulationstart >= core::Duration::Seconds(1))
		{
			lastaccumulationstart = accumulationstart;
			accumulationstart = core::Time::Now();
			lastframes = frames;
			frames = 0;
			lastframetime = frametime;
			frametime = core::Duration::Nanoseconds(0);
			lastuploadtime = uploadtime;
			uploadtime = core::Duration::Nanoseconds(0);
			lastrendertime = rendertime;
			rendertime = core::Duration::Nanoseconds(0);
			lastcomposetime = composetime;
			composetime = core::Duration::Nanoseconds(0);
		}
		// Write the data
		tbb::mutex::scoped_lock lock(statsmutex);
		this->stats = stats;
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

	scene::Model::Ptr GraphicsEngine::getModel(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Model", name);
		return scene::Model::Ptr((scene::Model*)res.get());
	}
	scene::Animation::Ptr GraphicsEngine::getAnimation(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Animation", name);
		return scene::Animation::Ptr((scene::Animation*)res.get());
	}
	scene::Terrain::Ptr GraphicsEngine::getTerrain(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Terrain", name);
		return scene::Terrain::Ptr((scene::Terrain*)res.get());
	}
	render::Pipeline::Ptr GraphicsEngine::getPipeline(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Pipeline", name);
		return render::Pipeline::Ptr((render::Pipeline*)res.get());
	}
	render::Material::Ptr GraphicsEngine::getMaterial(const std::string name)
	{
		res::Resource::Ptr res = rmgr->getOrLoad("Material", name);
		return render::Material::Ptr((render::Material*)res.get());
	}
	render::Mesh::Ptr GraphicsEngine::createMesh()
	{
		return driver->createMesh(uploadmgr);
	}

	render::Image *GraphicsEngine::takeScreenshot(int x, int y, int width, int height)
	{
		return driver->takeScreenshot(x, y, width, height);
	}

	void GraphicsEngine::setFileSystem(core::FileSystem::Ptr fs)
	{
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
