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

#ifndef _CORERENDER_RENDER_MESH_HPP_INCLUDED_
#define _CORERENDER_RENDER_MESH_HPP_INCLUDED_

#include "RenderObject.hpp"
#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"
#include "PrimitiveType.hpp"
#include "VertexLayout.hpp"

namespace cr
{
namespace render
{
	class Mesh : public RenderObject
	{
		public:
			Mesh(UploadManager &uploadmgr);
			~Mesh();

			void setVertices(VertexBuffer::Ptr vertices);
			VertexBuffer::Ptr getVertices()
			{
				return vertices;
			}

			void setIndices(IndexBuffer::Ptr indices);
			IndexBuffer::Ptr getIndices()
			{
				return indices;
			}

			void setVertexLayout(VertexLayout::Ptr layout);
			VertexLayout::Ptr getVertexLayout()
			{
				return layout;
			}

			void setIndexType(unsigned int indextype);
			unsigned int getIndexType()
			{
				return indextype;
			}

			void setStartIndex(unsigned int startindex);
			unsigned int getStartIndex()
			{
				return startindex;
			}

			void setIndexCount(unsigned int indexcount);
			unsigned int getIndexCount()
			{
				return indexcount;
			}

			void setBaseVertex(unsigned int basevertex);
			unsigned int getBaseVertex()
			{
				return basevertex;
			}

			void setVertexOffset(unsigned int vertexoffset);
			unsigned int getVertexOffset()
			{
				return vertexoffset;
			}

			void setVertexCount(unsigned int vertexcount);
			unsigned int getVertexCount()
			{
				return vertexcount;
			}

			void setPrimitiveType(PrimitiveType::List type);
			PrimitiveType::List getPrimitiveType()
			{
				return primitivetype;
			}

			void checkForUpdate();

			typedef core::SharedPointer<Mesh> Ptr;

			struct MeshData
			{
				VertexBuffer *vertices;
				IndexBuffer *indices;

				VertexLayout *layout;

				unsigned int layoutchangecounter;

				unsigned int indextype;
				unsigned int startindex;
				unsigned int indexcount;
				unsigned int basevertex;
				unsigned int vertexoffset;
				unsigned int vertexcount;

				PrimitiveType::List primitivetype;
			};
		protected:
			virtual void *getUploadData();
		private:
			VertexBuffer::Ptr vertices;
			IndexBuffer::Ptr indices;

			VertexLayout::Ptr layout;

			unsigned int layoutchangecounter;

			unsigned int indextype;
			unsigned int startindex;
			unsigned int indexcount;
			unsigned int basevertex;
			unsigned int vertexoffset;
			unsigned int vertexcount;

			PrimitiveType::List primitivetype;
	};
}
}

#endif
