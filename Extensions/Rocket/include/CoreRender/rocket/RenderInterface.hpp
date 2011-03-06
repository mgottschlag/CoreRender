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

#ifndef _CORERENDER_ROCKET_RENDERINTERFACE_HPP_INCLUDED_
#define _CORERENDER_ROCKET_RENDERINTERFACE_HPP_INCLUDED_

#include "CoreRender/render/VertexLayout.hpp"
#include "CoreRender/render/Material.hpp"
#include "CoreRender/render/VertexBuffer.hpp"
#include "CoreRender/render/IndexBuffer.hpp"
#include "CoreRender/render/Mesh.hpp"

#include <Rocket/Core.h>
#include <GameMath.hpp>

namespace cr
{
class GraphicsEngine;
namespace render
{
	class RenderQueue;
}
namespace rocket
{
	class RocketRenderable;

	class RenderInterface : public Rocket::Core::RenderInterface
	{
		public:
			RenderInterface(RocketRenderable *renderable,
			                GraphicsEngine *graphics,
			                render::Material::Ptr texturematerial,
			                render::Material::Ptr colormaterial);
			virtual ~RenderInterface();

			void shutdown();

			void reset();
			void finish();

			virtual void RenderGeometry(Rocket::Core::Vertex *vertices,
			                            int vertexcount,
			                            int *indices,
			                            int indexcount,
			                            Rocket::Core::TextureHandle texture,
			                            const Rocket::Core::Vector2f &translation);

			virtual void EnableScissorRegion(bool enable);
			virtual void SetScissorRegion(int x, int y, int width, int height);

			virtual bool LoadTexture(Rocket::Core::TextureHandle &texturehandle,
			                         Rocket::Core::Vector2i &texturedimensions,
			                         const Rocket::Core::String &source);
			virtual bool GenerateTexture(Rocket::Core::TextureHandle &texturehandle,
			                             const Rocket::Core::byte *source,
			                             const Rocket::Core::Vector2i &sourcedimensions);
			virtual void ReleaseTexture(Rocket::Core::TextureHandle texture);
		private:
			RocketRenderable *renderable;
			GraphicsEngine *graphics;
			render::Material::Ptr texturematerial;
			render::Material::Ptr colormaterial;

			void *vertexdata;
			void *indexdata;

			unsigned int vertexcount;
			unsigned int maxvertexcount;
			unsigned int indexdatasize;
			unsigned int maxindexdatasize;

			render::VertexBuffer::Ptr vertices;
			render::IndexBuffer::Ptr indices;
			render::VertexLayout::Ptr layout;

			std::vector<render::Mesh::Ptr> meshes;
			unsigned int meshcount;
	};
}
}

#endif
