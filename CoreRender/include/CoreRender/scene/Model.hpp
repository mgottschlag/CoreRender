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

#ifndef _CORERENDER_SCENE_MODEL_HPP_INCLUDED_
#define _CORERENDER_SCENE_MODEL_HPP_INCLUDED_

#include "../res/Resource.hpp"
#include "../math/Matrix4.hpp"
#include "../render/VertexBuffer.hpp"
#include "../render/IndexBuffer.hpp"
#include "../render/VertexLayout.hpp"
#include "../render/Material.hpp"
#include "GeometryFile.hpp"

#include <vector>

class TiXmlElement;

namespace cr
{
namespace render
{
	class RenderQueue;
	class Batch;
}
namespace scene
{
	class Model : public res::Resource
	{
		public:
			Model(cr::res::ResourceManager* rmgr, const std::string& name);
			virtual ~Model();

			void render(render::RenderQueue &queue,
			            math::Matrix4 transmat);
			void render(render::RenderQueue &queue,
			            unsigned int instancecount,
			            math::Matrix4 *transmat);

			struct Node
			{
				std::string name;
				int parent;
				math::Matrix4 transmat;
				math::Matrix4 abstrans;
				math::Matrix4 abstransinverse;
			};
			struct Joint
			{
				int node;
				math::Matrix4 jointmat;
			};
			struct BatchGeometry
			{
				/**
				 * Vertex layout.
				 */
				render::VertexLayout::Ptr layout;
				/**
				 * Index size, can be 1, 2 or 4.
				 */
				unsigned int indextype;
				/**
				 * Start index in the index buffer.
				 */
				unsigned int startindex;
				/**
				 * Number of indices in the index buffer.
				 */
				unsigned int indexcount;
				/**
				 * Base vertex (offset added to all indices).
				 */
				unsigned int basevertex;
				/**
				 * Byte offset to the first vertex.
				 */
				unsigned int vertexoffset;
				/**
				 * Number of vertices in the vertex buffer.
				 */
				unsigned int vertexcount;
				/**
				 * Joints influencing this batch.
				 */
				std::vector<Joint> joints;
			};

			struct Batch
			{
				unsigned int node;
				unsigned int geometry;
				render::Material::Ptr material;
			};

			int getNode(const std::string &name)
			{
				for (unsigned int i = 0; i < nodes.size(); i++)
				{
					if (nodes[i].name == name)
						return i;
				}
				return -1;
			}

			std::vector<BatchGeometry> &getGeometry()
			{
				return geometry;
			}
			std::vector<Node> &getNodes()
			{
				return nodes;
			}
			std::vector<Batch> &getBatches()
			{
				return batches;
			}
			render::VertexBuffer::Ptr getVertices()
			{
				return vertices;
			}
			render::IndexBuffer::Ptr getIndices()
			{
				return indices;
			}

			unsigned int getChangeCounter()
			{
				return changecounter;
			}

			render::Batch *prepareBatch(render::RenderQueue &queue,
			                            unsigned int batchindex,
			                            bool instancing,
			                            bool skinning);

			virtual bool load();

			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			virtual const char *getType()
			{
				return "Mesh";
			}

			typedef core::SharedPointer<Model> Ptr;
		private:
			bool loadGeometryFile(std::string filename);
			render::VertexLayout::Ptr createVertexLayout(const GeometryFile::AttribInfo &attribs);

			bool parseNode(TiXmlElement *xml, int parent);

			std::vector<BatchGeometry> geometry;
			std::vector<Node> nodes;
			std::vector<Batch> batches;
			render::VertexBuffer::Ptr vertices;
			render::IndexBuffer::Ptr indices;

			unsigned int changecounter;
	};
}
}

#endif
