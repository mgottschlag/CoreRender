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
#include "../math/BoundingBox.hpp"
#include "../render/VertexBuffer.hpp"
#include "../render/IndexBuffer.hpp"
#include "../render/VertexLayout.hpp"
#include "../render/Material.hpp"
#include "GeometryFile.hpp"
#include "../render/Mesh.hpp"

#include <GameMath.hpp>
#include <vector>

class TiXmlElement;

namespace cr
{
class GraphicsEngine;
namespace render
{
	struct RenderQueue;
	struct Batch;
}
namespace scene
{
	class Model : public res::Resource
	{
		public:
			Model(GraphicsEngine *graphics,
			      const std::string& name);
			virtual ~Model();

			void render(render::RenderQueue &queue,
			            math::Mat4f transmat);
			void render(render::RenderQueue &queue,
			            unsigned int instancecount,
			            math::Mat4f *transmat);

			struct Node
			{
				std::string name;
				int parent;
				math::Mat4f transmat;
				math::Mat4f abstrans;
				math::Mat4f abstransinverse;
			};
			struct Joint
			{
				int node;
				math::Mat4f jointmat;
			};
			struct BatchGeometry
			{
				/**
				 * Mesh which is used for this batch.
				 */
				render::Mesh::Ptr mesh;
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

			void setBoundingBox(const math::BoundingBox &boundingbox)
			{
				this->boundingbox = boundingbox;
			}
			math::BoundingBox getBoundingBox()
			{
				return boundingbox;
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
			void setVertexBuffer(render::VertexBuffer::Ptr vertices)
			{
				this->vertices = vertices;
			}
			render::VertexBuffer::Ptr getVertexBuffer()
			{
				return vertices;
			}
			void setIndexBuffer(render::IndexBuffer::Ptr indices)
			{
				this->indices = indices;
			}
			render::IndexBuffer::Ptr getIndexBuffer()
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

			GraphicsEngine *graphics;

			std::vector<BatchGeometry> geometry;
			std::vector<Node> nodes;
			std::vector<Batch> batches;
			render::VertexBuffer::Ptr vertices;
			render::IndexBuffer::Ptr indices;

			math::BoundingBox boundingbox;

			unsigned int changecounter;
	};
}
}

#endif
