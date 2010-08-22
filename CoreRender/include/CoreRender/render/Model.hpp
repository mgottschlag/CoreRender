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

class TiXmlElement;

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

			class Node
			{
				public:
					/**
					 * Constructor.
					 *
					 * Do not use this, use Model::addNode() instead.
					 */
					Node(const std::string &name, Model *model, Node *parent)
						: model(model), name(name), parent(parent)
					{
						if (parent)
							parent->addChild(this);
						model->nodes[name] = this;
					}
					~Node()
					{
						for (int i = children.size() - 1; i >= 0; i--)
						{
							delete children[i];
						}
						if (parent)
							parent->removeChild(this);
						std::map<std::string, Node*>::iterator it;
						it = model->nodes.find(name);
						if (it != model->nodes.end() && it->second == this)
							model->nodes.erase(it);
					}

					void setTransformation(const math::Matrix4 &transformation)
					{
						this->transformation = transformation;
					}
					const math::Matrix4 &getTransformation()
					{
						return transformation;
					}
					math::Matrix4 getAbsTrans()
					{
						// TODO: Optimize this
						if (parent)
							return parent->getAbsTrans() * transformation;
						else
							return transformation;
					}

					std::string getName()
					{
						return name;
					}
					Node *getParent()
					{
						return parent;
					}
				private:
					void addChild(Node *child)
					{
						children.push_back(child);
					}
					void removeChild(Node *child)
					{
						for (int i = children.size() - 1; i >= 0; i--)
						{
							if (children[i] == this)
								children.erase(children.begin() + i);
						}
					}

					Model *model;
					const std::string name;
					Node *parent;
					std::vector<Node*> children;
					math::Matrix4 transformation;
			};

			struct Joint
			{
				std::string name;
				math::Matrix4 jointmat;
				Node *node;
			};

			struct Batch
			{
				VertexLayout::Ptr layout;
				unsigned int indextype;
				unsigned int startindex;
				unsigned int indexcount;
				unsigned int basevertex;
				unsigned int vertexoffset;
				unsigned int vertexcount;
				std::vector<Joint> joints;
			};

			struct Mesh
			{
				unsigned int batch;
				Material::Ptr material;
				Node *node;
			};

			void addBatch(const Batch &batch);
			Batch *getBatch(unsigned int index);
			void removeBatch(unsigned int index);
			unsigned int getBatchCount();

			void addMesh(const Mesh &mesh);
			Mesh *getMesh(unsigned int index);
			void removeMesh(unsigned int index);
			unsigned int getMeshCount();

			Model::Node *addNode(const std::string &name, Node *parent);
			void removeNode(const std::string &name);
			void removeNode(Node *node);
			Node *getRootNode();
			Node *getNode(const std::string &name);

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

			bool parseNode(TiXmlElement *xml, Node *parent);

			render::VideoDriver *driver;
			render::Renderer *renderer;

			IndexBuffer::Ptr indexbuffer;
			VertexBuffer::Ptr vertexbuffer;

			std::vector<Batch> batches;
			std::vector<Mesh> meshes;
			Node *rootnode;
			std::map<std::string, Node*> nodes;

			friend class Node;
	};
}
}

#endif
