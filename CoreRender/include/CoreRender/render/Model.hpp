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
	/**
	 * Class containing several meshes and joint information.
	 *
	 * A model contains a node hierarchy, and every node can contain zero or more
	 * meshes. The nodes also are used for animation as joints, as every node
	 * contains a transformation and joints just link to a certain node (by
	 * name).
	 */
	class Model : public res::Resource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Model(res::ResourceManager *rmgr,
			      const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Model();

			/**
			 * Model node holding zero or more meshes and zero or more joints.
			 */
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
					/**
					 * Destructor.
					 */
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

					/**
					 * Sets the relative transformation of the model node.
					 * @param transformation Transformation relative to the
					 * parent.
					 */
					void setTransformation(const math::Matrix4 &transformation)
					{
						this->transformation = transformation;
						transinverse = transformation.inverse();
					}
					/**
					 * Returns the transformation relative to the parent.
					 * @return Relative transformation.
					 */
					const math::Matrix4 &getTransformation()
					{
						return transformation;
					}
					/**
					 * Returns the transformation relative to the parent.
					 * @return Relative transformation.
					 */
					const math::Matrix4 &getInvTrans()
					{
						return transinverse;
					}
					/**
					 * Returns the absolute transformation of the model node.
					 * @return Absolute transformation.
					 */
					math::Matrix4 getAbsTrans()
					{
						// TODO: Optimize this
						if (parent)
							return parent->getAbsTrans() * transformation;
						else
							return transformation;
					}

					/**
					 * Returns the name of the node.
					 * @return Name.
					 */
					std::string getName()
					{
						return name;
					}
					/**
					 * Returns the parent of the node.
					 * @return Parent.
					 */
					Node *getParent()
					{
						return parent;
					}
					/**
					 * Returns a list of all children of this node.
					 * @return List of all children.
					 */
					const std::vector<Node*> &getChildren()
					{
						return children;
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
					math::Matrix4 transinverse;
			};
			/**
			 * Stripped-down version of Node to be used within ModelRenderable
			 * for animations.
			 */
			struct AnimationNode
			{
				/**
				 * Returns the absolute transformation of the model node.
				 * @return Absolute transformation.
				 */
				math::Matrix4 getAbsTrans()
				{
					return abstrans;
				}

				/**
				 * Computes the absolute transformation of the node if dirty is
				 * set to true.
				 */
				void computeAbsTrans()
				{
					if (!dirty)
						return;
					if (parent)
					{
						if (parent->dirty)
							parent->computeAbsTrans();
						abstrans = parent->getAbsTrans() * transformation;
						abstransinverse = transinverse * parent->abstransinverse;
					}
					else
					{
						abstrans = transformation;
						abstransinverse = transinverse;
					}
					dirty = false;
				}

				/**
				 * Name of the model node.
				 */
				std::string name;
				/**
				 * Parent of this node, if 0, then this node is the root node.
				 */
				AnimationNode *parent;
				/**
				 * Transformation of this node relative to the parent.
				 */
				math::Matrix4 transformation;
				/**
				 * Inverse of transformation.
				 */
				math::Matrix4 transinverse;
				/**
				 * Sets whether the absolute transformation needs to be updated.
				 */
				bool dirty;
				/**
				 * Absolute transformation.
				 */
				math::Matrix4 abstrans;
				/**
				 * Absolute transformation (inverse).
				 */
				math::Matrix4 abstransinverse;
			};

			/**
			 * Joint information.
			 */
			struct Joint
			{
				/**
				 * Name of the joint. This is the name of the node it is linked
				 * to.
				 */
				std::string name;
				/**
				 * Inverse joint bind matrix. Multiply vertices with this matrix
				 * to get their position in bone space.
				 */
				math::Matrix4 jointmat;
				/**
				 * Node this joint is linked to.
				 */
				Node *node;
			};

			/**
			 * Batch of geometry to be used in Mesh. One batch can be used by
			 * multiple meshes with different transformation.
			 *
			 * For a better description of most these fields see RenderJob.
			 */
			struct Batch
			{
				/**
				 * Vertex layout.
				 */
				VertexLayout::Ptr layout;
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

			/**
			 * Model mesh, contains a batch and a material.
			 */
			struct Mesh
			{
				/**
				 * Batch to be used for this mesh.
				 */
				unsigned int batch;
				/**
				 * Material to be used for this mesh.
				 */
				Material::Ptr material;
				/**
				 * Node which is used for transformation of the mesh.
				 */
				Node *node;
			};

			/**
			 * Adds a batch to the model.
			 * @param batch Batch information.
			 */
			void addBatch(const Batch &batch);
			/**
			 * Returns the batch at a certain index.
			 * @param index Batch index.
			 * @return Batch at the index, or 0 if the batch was not found.
			 */
			Batch *getBatch(unsigned int index);
			/**
			 * Removes the batch at a certain index.
			 */
			void removeBatch(unsigned int index);
			/**
			 * Returns the number of batches in the model.
			 */
			unsigned int getBatchCount();

			/**
			 * Adds a mesh to the model.
			 * @param mesh Mesh information.
			 */
			void addMesh(const Mesh &mesh);
			/**
			 * Returns the mesh at a certain index.
			 * @param index mesh index.
			 * @return Mesh at the index, or 0 if the mesh was not found.
			 */
			Mesh *getMesh(unsigned int index);
			/**
			 * Removes the mesh at a certain index.
			 */
			void removeMesh(unsigned int index);
			/**
			 * Returns the number of meshes in the model.
			 */
			unsigned int getMeshCount();

			/**
			 * Inserts a new model node into the node hierarchy.
			 * @param name Name of the new node.
			 * @param parent Parent of the node, if this is 0, the root node
			 * will be replaced with the node.
			 * @return New node.
			 */
			Model::Node *addNode(const std::string &name, Node *parent);
			/**
			 * Removes a node from the model.
			 * @param name Node name.
			 */
			void removeNode(const std::string &name);
			/**
			 * @param node Node to be removed.
			 */
			void removeNode(Node *node);
			/**
			 * Returns the root model node.
			 * @return Root mode node.
			 */
			Node *getRootNode();
			/**
			 * Returns the node with a certain name.
			 * @return Node, or 0 if the node was not found.
			 */
			Node *getNode(const std::string &name);
			/**
			 * Returns a map with all nodes usable for animation.
			 * @param nodelist List which is filled with all mode nodes.
			 */
			void getNodeList(std::map<std::string, AnimationNode*> &nodelist);

			/**
			 * Sets the index buffer used for all batches in the model.
			 * @param indexbuffer Model index buffer.
			 */
			void setIndexBuffer(IndexBuffer::Ptr indexbuffer);
			/**
			 * Returns the index buffer used for this model.
			 * @return Index buffer.
			 */
			IndexBuffer::Ptr getIndexBuffer();
			/**
			 * Sets the vertex buffer used for all batches in the model.
			 * @param vertexbuffer Model vertex buffer.
			 */
			void setVertexBuffer(VertexBuffer::Ptr vertexbuffer);
			/**
			 * Returns the vertex buffer used for this model.
			 * @return Vertex buffer.
			 */
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
