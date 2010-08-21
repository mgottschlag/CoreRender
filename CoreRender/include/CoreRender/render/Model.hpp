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

#ifndef _CORERENDER_RENDER_MODEL_HPP_INCLUDED_
#define _CORERENDER_RENDER_MODEL_HPP_INCLUDED_

#include "Material.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "VertexLayout.hpp"
#include "GeometryFile.hpp"

namespace cr
{
namespace render
{
	class Model : public res::Resource
	{
		public:
			Model(render::VideoDriver *driver,
			      render::Renderer *renderer,
			      res::ResourceManager *rmgr,
			      const std::string &name);
			virtual ~Model();

			struct Batch
			{
				VertexLayout::Ptr layout;
				unsigned int indextype;
				unsigned int startindex;
				unsigned int indexcount;
				unsigned int basevertex;
				unsigned int vertexoffset;
				unsigned int vertexcount;
			};

			void addBatch(const Batch &batch);
			Batch *getBatch(unsigned int index);
			void removeBatch(unsigned int index);
			unsigned int getBatchCount();

			struct Mesh
			{
				unsigned int batch;
				Material::Ptr material;
				// TODO: Transformation
			};

			void addMesh(const Mesh &mesh);
			Mesh *getMesh(unsigned int index);
			void removeMesh(unsigned int index);
			unsigned int getMeshCount();

			void setIndexBuffer(IndexBuffer::Ptr indexbuffer);
			IndexBuffer::Ptr getIndexBuffer();
			void setVertexBuffer(VertexBuffer::Ptr indexbuffer);
			VertexBuffer::Ptr getVertexBuffer();

			virtual bool load();

			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			virtual const char *getType()
			{
				return "Model";
			}

			typedef core::SharedPointer<Model> Ptr;
		private:
			bool loadGeometryFile(std::string filename);
			VertexLayout::Ptr createVertexLayout(const GeometryFile::AttribInfo &attribs);

			render::VideoDriver *driver;
			render::Renderer *renderer;

			IndexBuffer::Ptr indexbuffer;
			VertexBuffer::Ptr vertexbuffer;

			std::vector<Batch> batches;
			std::vector<Mesh> meshes;
	};
}
}

#endif
