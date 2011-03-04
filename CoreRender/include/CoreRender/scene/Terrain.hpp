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

#ifndef _CORERENDER_SCENE_TERRAIN_HPP_INCLUDED_
#define _CORERENDER_SCENE_TERRAIN_HPP_INCLUDED_

#include "../res/Resource.hpp"
#include <GameMath.hpp>
#include "../render/Texture.hpp"
#include "../render/VertexBuffer.hpp"
#include "../render/IndexBuffer.hpp"
#include "../render/FrameData.hpp"
#include "../render/Material.hpp"
#include "../render/VertexLayout.hpp"
#include "../render/Mesh.hpp"

namespace cr
{
class GraphicsEngine;
namespace scene
{
	class TerrainMaterial;

	class Terrain : public res::Resource
	{
		public:
			Terrain(GraphicsEngine *graphics, const std::string& name);
			virtual ~Terrain();

			void render(render::RenderQueue &queue,
			            math::Mat4f transmat,
			            math::Vec3f camerapos);

			void set(unsigned int sizex,
			         unsigned int sizez,
			         unsigned int patchsize = 33,
			         float *displacement = 0);

			void setDisplacement(unsigned int x,
			                     unsigned int z,
			                     math::Vec3f displacement);
			void setDisplacement(unsigned int x,
			                     unsigned int z,
			                     float height);
			math::Vec3f getDisplacement(int x, int z);

			/**
			 * Discards the displacement data in RAM and only keeps the copy in
			 * VRAM. If you call this, following calls to setDisplacement()
			 * and getDisplacement() will not work properly.
			 */
			void discardDisplacementData();

			math::Vec2u getSize()
			{
				return math::Vec2u(sizex, sizez);
			}

			void setMaterial(render::Material::Ptr material)
			{
				this->material = material;
				material->addTexture("displacementMap", displacementmap);
			}

			virtual bool load();

			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			virtual const char *getType()
			{
				return "Terrain";
			}

			typedef core::SharedPointer<Terrain> Ptr;
		private:
			void updateNormals(unsigned int x,
			                   unsigned int z,
			                   unsigned int sizex,
			                   unsigned int sizez);
			void updateGeometry(unsigned int patchsize);

			void renderPatch(render::RenderQueue &queue,
			                 math::Mat4f transmat,
			                 unsigned int lod,
			                 float *offsetscale,
			                 render::CustomUniform &texcoordscale);
			void renderRecursively(render::RenderQueue &queue,
			                       math::Mat4f transmat,
			                       unsigned int maxdepth,
			                       math::Vec3f camerapos,
			                       float *offsetscale,
			                       render::CustomUniform &texcoordscale);

			GraphicsEngine *graphics;

			render::Material::Ptr material;

			float *displacement;
			float *normals;
			unsigned int sizex;
			unsigned int sizez;
			unsigned int patchsize;

			struct Patch
			{
				unsigned int patchsize;
				unsigned int indexoffset;
				render::Mesh::Ptr mesh;
			};
			std::vector<Patch> patches;
			render::VertexBuffer::Ptr vertices;
			render::IndexBuffer::Ptr indices;
			render::VertexLayout::Ptr layout;

			render::Texture::Ptr displacementmap;
			render::Texture::Ptr normalmap;

			tbb::mutex mutex;
	};
}
}

#endif
