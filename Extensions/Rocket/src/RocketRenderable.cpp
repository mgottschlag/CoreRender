/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CoreRender/rocket/RocketRenderable.hpp"
#include "CoreRender/rocket/RenderInterface.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/GraphicsEngine.hpp"

namespace cr
{
namespace rocket
{
	RocketRenderable::RocketRenderable(GraphicsEngine *graphics,
	                                   render::Material::Ptr texturematerial,
	                                   render::Material::Ptr colormaterial)
		: fileinterface(graphics->getFileSystem())
	{
		renderinterface = new RenderInterface(this,
		                                      graphics,
		                                      texturematerial,
		                                      colormaterial);
		renderinterface->AddReference();
	}
	RocketRenderable::~RocketRenderable()
	{
		renderinterface->shutdown();
		renderinterface->RemoveReference();
	}

	void RocketRenderable::reset()
	{
		renderinterface->reset();
		scissorenabled = false;
		scissorentries.clear();
		renderentries.clear();
	}
	void RocketRenderable::render(render::RenderQueue &queue,
	                              math::Mat4f transmat)
	{
		renderinterface->finish();
		for (unsigned int i = 0; i < renderentries.size(); i++)
		{
			render::Batch *batch = queue.prepareBatch(renderentries[i].material.get(),
		                                              renderentries[i].mesh.get(),
		                                              false,
		                                              false);
			if (!batch)
				continue;
			batch->transmat = transmat;
			// Translation
			// TODO: This overwrites existing material uniforms
			//std::cout << "translation: " << renderentries[i].translation.x << "/" << renderentries[i].translation.y << std::endl;
			core::MemoryPool *memory = queue.memory;
			batch->customuniformcount = 1;
			void *ptr = memory->allocate(sizeof(render::CustomUniform));
			render::CustomUniform *uniform = (render::CustomUniform*)ptr;
			char *uniformname = (char*)memory->allocate(strlen("translation") + 1);
			strcpy(uniformname, "translation");
			float *uniformdata = (float*)memory->allocate(sizeof(float) * 2);
			uniformdata[0] = renderentries[i].translation.x;
			uniformdata[1] = renderentries[i].translation.y;
			uniform->name = uniformname;
			uniform->size = 2;
			uniform->data = uniformdata;
			batch->customuniforms = uniform;
			// Add batch to the render queue
			tbb::mutex::scoped_lock lock(queue.batchmutex);
			queue.batches.push_back(batch);
		}
	}

	void RocketRenderable::renderMesh(render::Mesh::Ptr mesh,
	                                  render::Material::Ptr material,
	                                  math::Vec2i translation)
	{
		RenderEntry entry;
		entry.mesh = mesh;
		entry.material = material;
		entry.translation = translation;
		if (scissorenabled)
			entry.scissor = scissorentries.size() - 1;
		else
			entry.scissor = -1;
		renderentries.push_back(entry);
	}

	void RocketRenderable::setScissorsEnabled(bool enabled)
	{
		scissorenabled = enabled;
	}
	void RocketRenderable::setScissorRegion(int x, int y, int width, int height)
	{
		ScissorRegion region;
		region.x = x;
		region.y = y;
		region.width = width;
		region.height = height;
		scissorentries.push_back(region);
	}
}
}