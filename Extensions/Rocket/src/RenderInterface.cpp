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

#include "CoreRender/rocket/RenderInterface.hpp"
#include "CoreRender/rocket/RocketRenderable.hpp"
#include "CoreRender/GraphicsEngine.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/rocket/RocketTexture.hpp"

#include <cassert>

namespace cr
{
namespace rocket
{
	const unsigned int VERTEXSIZE = 5 * sizeof(float);

	RenderInterface::RenderInterface(RocketRenderable *renderable,
	                                 GraphicsEngine *graphics,
	                                 render::Material::Ptr texturematerial,
	                                 render::Material::Ptr colormaterial)
		: renderable(renderable), graphics(graphics),
		texturematerial(texturematerial), colormaterial(colormaterial)
	{
		res::ResourceManager *rmgr = graphics->getResourceManager();
		res::NameRegistry &names = rmgr->getNameRegistry();
		vertices = rmgr->createResource<render::VertexBuffer>("VertexBuffer");
		indices = rmgr->createResource<render::IndexBuffer>("IndexBuffer");
		layout = new render::VertexLayout(rmgr->getUploadManager(), 3);
		layout->setElement(0,
		                   names.getAttrib("pos"),
		                   0,
		                   2,
		                   0,
		                   render::VertexElementType::Float,
		                   VERTEXSIZE);
		layout->setElement(1,
		                   names.getAttrib("color"),
		                   0,
		                   4,
		                   2 * sizeof(float),
		                   render::VertexElementType::UnsignedByte,
		                   VERTEXSIZE);
		layout->setElement(2,
		                   names.getAttrib("texcoord0"),
		                   0,
		                   2,
		                   3 * sizeof(float),
		                   render::VertexElementType::Float,
		                   VERTEXSIZE);
		// Reset vertex/index data
		reset();
	}
	RenderInterface::~RenderInterface()
	{
		if (vertexdata)
			free(vertexdata);
		if (indexdata)
			free(indexdata);
	}

	void RenderInterface::shutdown()
	{
		if (vertexdata)
			free(vertexdata);
		vertexdata = 0;
		if (indexdata)
			free(indexdata);
		indexdata = 0;
		renderable = 0;
		layout = 0;
		vertices = 0;
		indices = 0;
		texturematerial = 0;
		colormaterial = 0;
		meshcount = 0;
		meshes.clear();
	}

	void RenderInterface::reset()
	{
		vertexdata = 0;
		vertexcount = 0;
		maxvertexcount = 0;
		indexdata = 0;
		indexdatasize = 0;
		maxindexdatasize = 0;
		meshcount = 0;
	}
	void RenderInterface::finish()
	{
		// Update buffers
		if (indexdatasize > 0)
		{
			vertices->set(vertexcount * VERTEXSIZE, vertexdata, render::VertexBufferUsage::Dynamic, false);
			indices->set(indexdatasize, indexdata, false);
			reset();
		}
	}

	void RenderInterface::RenderGeometry(Rocket::Core::Vertex *vertices,
	                                     int vertexcount,
	                                     int *indices,
	                                     int indexcount,
	                                     Rocket::Core::TextureHandle texture,
	                                     const Rocket::Core::Vector2f &translation)
	{
		if (!renderable)
			return;
		assert(VERTEXSIZE == sizeof(Rocket::Core::Vertex));
		unsigned int indextype = 1;
		if (vertexcount > 65535)
			indextype = 4;
		else if (vertexcount > 255)
			indextype = 2;
		unsigned int indexdatasize = indexcount * indextype;
		// Round up to 4 bytes
		indexdatasize = (indexdatasize + 3) & ~3;
		// Ensure that there is enough memory available
		if (this->vertexcount + vertexcount > maxvertexcount)
		{
			maxvertexcount = std::max((this->vertexcount + vertexcount) * 3 / 2, 32u);
			vertexdata = realloc(vertexdata, maxvertexcount * VERTEXSIZE);
		}
		if (this->indexdatasize + indexdatasize > maxindexdatasize)
		{
			maxindexdatasize = std::max((this->indexdatasize + indexdatasize) * 3 / 2, 256u);
			indexdata = realloc(indexdata, maxindexdatasize);
		}
		// Store indices/vertices
		unsigned int vertexoffset = this->vertexcount * VERTEXSIZE;
		unsigned int indexoffset = this->indexdatasize / indextype;
		memcpy((char*)vertexdata + vertexoffset,
		       vertices,
		       vertexcount * VERTEXSIZE);
		switch (indextype)
		{
			case 1:
				for (int i = 0; i < indexcount; i++)
				{
					((unsigned char*)indexdata)[indexoffset + i] = indices[i];
				}
				break;
			case 2:
				for (int i = 0; i < indexcount; i++)
				{
					((unsigned short*)indexdata)[indexoffset + i] = indices[i];
				}
				break;
			case 4:
				for (int i = 0; i < indexcount; i++)
				{
					((unsigned int*)indexdata)[indexoffset + i] = indices[i];
				}
				break;
		}
		this->vertexcount += vertexcount;
		this->indexdatasize += indexdatasize;
		// Create mesh
		if (meshcount >= meshes.size())
		{
			meshes.push_back(graphics->createMesh());
			meshes[meshcount]->setVertexBuffer(this->vertices);
			meshes[meshcount]->setIndexBuffer(this->indices);
			meshes[meshcount]->setVertexLayout(layout);
		}
		render::Mesh *mesh = meshes[meshcount].get();
		meshcount++;
		mesh->setIndexType(indextype);
		mesh->setVertexOffset(vertexoffset);
		mesh->setIndexCount(indexcount);
		mesh->setPrimitiveType(render::PrimitiveType::Triangle);
		mesh->setStartIndex(indexoffset);
		mesh->setVertexCount(vertexcount);
		// Get the material
		render::Material::Ptr material;
		RocketTexture *rockettexture = (RocketTexture*)texture;
		if (rockettexture)
			material = rockettexture->getMaterial();
		else
			material = colormaterial;
		// Render the mesh
		renderable->renderMesh(mesh, material, math::Vec2i(translation.x, translation.y));
	}

	void RenderInterface::EnableScissorRegion(bool enable)
	{
		if (!renderable)
			return;
		renderable->setScissorsEnabled(enable);
	}
	void RenderInterface::SetScissorRegion(int x, int y, int width, int height)
	{
		if (!renderable)
			return;
		renderable->setScissorRegion(x, y, width, height);
	}

	bool RenderInterface::LoadTexture(Rocket::Core::TextureHandle &texturehandle,
	                                  Rocket::Core::Vector2i &texturedimensions,
	                                  const Rocket::Core::String &source)
	{
		if (!renderable)
			return false;
		render::Texture::Ptr texture = graphics->getResourceManager()->getOrLoad<render::Texture>("Texture", source.CString());
		if (!texture->waitForLoading(false))
			return false;
		texturedimensions.x = texture->getWidth();
		texturedimensions.y = texture->getHeight();
		res::Resource::Ptr materialres = texturematerial->clone();
		render::Material::Ptr material = (render::Material*)materialres.get();
		material->addTexture("tex", texture);
		RocketTexture *rockettexture = new RocketTexture(texture, material);
		rockettexture->grab();
		texturehandle = (uintptr_t)rockettexture;
		return true;
	}
	bool RenderInterface::GenerateTexture(Rocket::Core::TextureHandle &texturehandle,
	                                      const Rocket::Core::byte *source,
	                                      const Rocket::Core::Vector2i &sourcedimensions)
	{
		if (!renderable)
			return false;
		render::Texture::Ptr texture = graphics->getResourceManager()->createResource<render::Texture>("Texture");
		texture->set2D(sourcedimensions.x,
		               sourcedimensions.y,
		               render::TextureFormat::RGBA8,
		               render::TextureFormat::RGBA8,
		               (void*)source);
		res::Resource::Ptr materialres = texturematerial->clone();
		render::Material::Ptr material = (render::Material*)materialres.get();
		material->addTexture("tex", texture);
		RocketTexture *rockettexture = new RocketTexture(texture, material);
		rockettexture->grab();
		texturehandle = (uintptr_t)rockettexture;
		return true;
	}
	void RenderInterface::ReleaseTexture(Rocket::Core::TextureHandle texture)
	{
		RocketTexture *rockettexture = (RocketTexture*)texture;
		rockettexture->drop();
	}

}
}
