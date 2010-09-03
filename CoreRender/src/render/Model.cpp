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

#include "CoreRender/render/Model.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/VideoDriver.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>
#include <queue>

namespace cr
{
namespace render
{
	Model::Model(render::Renderer *renderer,
	             res::ResourceManager *rmgr,
	             const std::string &name)
		: Resource(rmgr, name), renderer(renderer), rootnode(0)
	{
	}
	Model::~Model()
	{
	}

	void Model::addBatch(const Model::Batch &batch)
	{
		batches.push_back(batch);
	}
	Model::Batch *Model::getBatch(unsigned int index)
	{
		if (index >= batches.size())
			return 0;
		return &batches[index];
	}
	void Model::removeBatch(unsigned int index)
	{
		if (index >= batches.size())
			return;
		batches.erase(batches.begin() + index);
	}
	unsigned int Model::getBatchCount()
	{
		return batches.size();
	}

	void Model::addMesh(const Model::Mesh &mesh)
	{
		meshes.push_back(mesh);
	}
	Model::Mesh *Model::getMesh(unsigned int index)
	{
		if (index >= meshes.size())
			return 0;
		return &meshes[index];
	}
	void Model::removeMesh(unsigned int index)
	{
		if (index >= meshes.size())
			return;
		meshes.erase(meshes.begin() + index);
	}
	unsigned int Model::getMeshCount()
	{
		return meshes.size();
	}

	Model::Node *Model::addNode(const std::string &name, Model::Node *parent)
	{
		Node *node = new Node(name, this, parent);
		if (!parent)
		{
			// Set the node as the new root node
			if (rootnode)
				delete rootnode;
			rootnode = node;
		}
		return node;
	}
	void Model::removeNode(const std::string &name)
	{
		Node *node = getNode(name);
		if (node)
			delete node;
	}
	void Model::removeNode(Model::Node *node)
	{
		delete node;
	}
	Model::Node *Model::getRootNode()
	{
		return rootnode;
	}
	Model::Node *Model::getNode(const std::string &name)
	{
		std::map<std::string, Node*>::iterator it = nodes.find(name);
		if (it == nodes.end())
			return 0;
		return it->second;
	}
	void Model::getNodeList(std::map<std::string, Model::AnimationNode*> &nodelist)
	{
		std::queue<Node*> currentnodes;
		currentnodes.push(rootnode);
		while (currentnodes.size() > 0)
		{
			Node *node = currentnodes.front();
			currentnodes.pop();
			AnimationNode *newnode = new AnimationNode;
			newnode->name = node->getName();
			if (node->getParent())
				newnode->parent = nodelist.find(node->getParent()->getName())->second;
			else
				newnode->parent = 0;
			newnode->transformation = node->getTransformation();
			nodelist.insert(std::make_pair(newnode->name, newnode));
			// Child nodes
			const std::vector<Node*> &children = node->getChildren();
			for (unsigned int i = 0; i < children.size(); i++)
			{
				currentnodes.push(children[i]);
			}
		}
	}

	void Model::setIndexBuffer(IndexBuffer::Ptr indexbuffer)
	{
		this->indexbuffer = indexbuffer;
	}
	IndexBuffer::Ptr Model::getIndexBuffer()
	{
		return indexbuffer;
	}
	void Model::setVertexBuffer(VertexBuffer::Ptr vertexbuffer)
	{
		this->vertexbuffer = vertexbuffer;
	}
	VertexBuffer::Ptr Model::getVertexBuffer()
	{
		return vertexbuffer;
	}

	bool Model::load()
	{
		std::string path = getPath();
		std::string directory = core::FileSystem::getDirectory(path);
		// Open XML file
		TiXmlDocument xml(path.c_str());
		if (!loadResourceFile(xml))
		{
			finishLoading(false);
			return false;
		}
		// Load XML file
		TiXmlNode *root = xml.FirstChild("Model");
		if (!root || !root->ToElement())
		{
			getManager()->getLog()->error("%s: <Model> not found.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		// Get geometry file
		const char *geofilename = root->ToElement()->Attribute("geometry");
		if (!geofilename)
		{
			getManager()->getLog()->error("%s: No geometry file specified.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		// Open geometry file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		if (!loadGeometryFile(fs->getPath(geofilename, directory)))
		{
			finishLoading(false);
			return false;
		}
		// Load nodes
		TiXmlElement *rootnodeelem = root->FirstChildElement("Node");
		if (!rootnodeelem)
		{
			getManager()->getLog()->error("%s: No node available.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		if (!parseNode(rootnodeelem, 0))
		{
			finishLoading(false);
			return false;
		}
		// Load joints
		for (TiXmlElement *element = root->FirstChildElement("Armature");
		     element != 0;
		     element = element->NextSiblingElement("Armature"))
		{
			// Get batch index
			const char *batchstr = element->Attribute("batch");
			if (!batchstr)
			{
				getManager()->getLog()->warning("%s: Armature batch missing.",
				                                getName().c_str());
				continue;
			}
			unsigned int batchidx = atoi(batchstr);
			if (batchidx >= batches.size())
			{
				getManager()->getLog()->warning("%s: Invalid armature batch.",
				                                getName().c_str());
				continue;
			}
			// Load joints
			for (TiXmlElement *jointelem = element->FirstChildElement("Joint");
			     jointelem != 0;
			     jointelem = jointelem->NextSiblingElement("Joint"))
			{
				// Get and check joint index
				const char *indexstr = jointelem->Attribute("index");
				if (!indexstr)
				{
					getManager()->getLog()->warning("%s: Joint index missing.",
					                                getName().c_str());
					continue;
				}
				unsigned int index = atoi(indexstr);
				if (index >= batches[batchidx].joints.size())
				{
					getManager()->getLog()->warning("%s: Joint index invalid.",
					                                getName().c_str());
					continue;
				}
				// Get and check joint name
				const char *name = jointelem->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->warning("%s: Joint name missing.",
					                                getName().c_str());
					continue;
				}
				Node *node = getNode(name);
				if (!node)
				{
					getManager()->getLog()->warning("%s: Joint node \"%s\" not found.",
					                                getName().c_str(), name);
					continue;
				}
				batches[batchidx].joints[index].name = name;
				batches[batchidx].joints[index].node = node;
			}
		}
		finishLoading(true);
		return true;
	}

	bool Model::loadGeometryFile(std::string filename)
	{
		// Open file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		core::File::Ptr file = fs->open(filename, core::FileAccess::Read);
		if (!file)
		{
			getManager()->getLog()->error("Could not open file \"%s\".",
			                              filename.c_str());
			return false;
		}
		// Read header
		// TODO: Adapt all this to big-endian
		GeometryFile::Header header;
		if (file->read(sizeof(header), &header) != sizeof(header))
		{
			getManager()->getLog()->error("%s: Could not read geometry header.",
			                              getName().c_str());
			return false;
		}
		if (header.tag != GeometryFile::tag
		 || header.version != GeometryFile::version)
		{
			getManager()->getLog()->error("%s: Invalid geometry file.",
			                              getName().c_str());
			return false;
		}
		// Read vertex and index data
		void *vertexdata = malloc(header.vertexdatasize);
		if (file->read(header.vertexdatasize, vertexdata) != (int)header.vertexdatasize)
		{
			getManager()->getLog()->error("%s: Could not read vertex data.",
			                              getName().c_str());
			free(vertexdata);
			return false;
		}
		void *indexdata = malloc(header.indexdatasize);
		if (file->read(header.indexdatasize, indexdata) != (int)header.indexdatasize)
		{
			getManager()->getLog()->error("%s: Could not read index data.",
			                              getName().c_str());
			free(vertexdata);
			free(indexdata);
			return false;
		}
		// Construct vertex/index buffers
		res::ResourceManager *rmgr = getManager();
		vertexbuffer = rmgr->createResource<VertexBuffer>("VertexBuffer");
		indexbuffer = rmgr->createResource<IndexBuffer>("IndexBuffer");
		if (!vertexbuffer || !indexbuffer)
		{
			getManager()->getLog()->error("%s: Could not create buffers.",
			                              getName().c_str());
			free(vertexdata);
			free(indexdata);
			return false;
		}
		vertexbuffer->set(header.vertexdatasize,
		                  vertexdata,
		                  VertexBufferUsage::Static,
		                  false);
		indexbuffer->set(header.indexdatasize, indexdata, false);
		// Read batch info
		std::vector<GeometryFile::Batch> batchdata(header.batchcount);
		for (unsigned int i = 0; i < header.batchcount; i++)
		{
			GeometryFile::AttribInfo &attribs = batchdata[i].attribs;
			GeometryFile::GeometryInfo &geom = batchdata[i].geom;
			if (file->read(sizeof(attribs), &attribs) != sizeof(attribs)
			 || file->read(sizeof(geom), &geom) != sizeof(geom))
			{
				getManager()->getLog()->error("%s: Could not read batch.",
				                              getName().c_str());
				return false;
			}
			// Joint matrices
			if (geom.jointcount == 0)
				continue;
			int jointsize = sizeof(float) * geom.jointcount * 16;
			batchdata[i].jointmatrices = std::vector<float>(geom.jointcount * 16);
			if (file->read(jointsize, &batchdata[i].jointmatrices[0]) != jointsize)
			{
				getManager()->getLog()->error("%s: Could not joint matrices.",
				                              getName().c_str());
				return false;
			}
		}
		// Construct batch info
		std::vector<Batch> batches(header.batchcount);
		for (unsigned int i = 0; i < header.batchcount; i++)
		{
			batches[i].layout = createVertexLayout(batchdata[i].attribs);
			if (!batches[i].layout)
			{
				getManager()->getLog()->error("%s: Could not create vertex layout.",
				                              getName().c_str());
				return false;
			}
			GeometryFile::AttribInfo &attribs = batchdata[i].attribs;
			GeometryFile::GeometryInfo &geom = batchdata[i].geom;
			batches[i].basevertex = geom.basevertex;
			batches[i].indextype = geom.indextype;
			batches[i].indexcount = geom.indexcount;
			batches[i].startindex = geom.indexoffset / geom.indextype;
			batches[i].vertexoffset = geom.vertexoffset;
			batches[i].vertexcount = geom.vertexsize / attribs.stride;
			// Create joints
			batches[i].joints = std::vector<Joint>(batchdata[i].geom.jointcount);
			for (unsigned int j = 0; j < batchdata[i].geom.jointcount; j++)
			{
				math::Matrix4 &jointmat = batches[i].joints[j].jointmat;
				memcpy(&jointmat.m[0],
				       &batchdata[i].jointmatrices[j * 16],
				       sizeof(float) * 16);
			}
		}
		this->batches = batches;
		return true;
	}

	VertexLayout::Ptr Model::createVertexLayout(const GeometryFile::AttribInfo &attribs)
	{
		if (attribs.texcoordcount > GeometryFile::maxtexcoords)
			return 0;
		if (attribs.colorcount > GeometryFile::maxcolors)
			return 0;
		// Get element count
		unsigned int elemcount = 0;
		if (attribs.flags & GeometryFile::AttribFlags::HasPositions)
			elemcount++;
		if (attribs.flags & GeometryFile::AttribFlags::HasNormals)
			elemcount++;
		if (attribs.flags & GeometryFile::AttribFlags::HasTangents)
			elemcount++;
		if (attribs.flags & GeometryFile::AttribFlags::HasBitangents)
			elemcount++;
		if (attribs.flags & GeometryFile::AttribFlags::HasJoints)
			elemcount += 2;
		elemcount += attribs.texcoordcount;
		elemcount += attribs.colorcount;
		if (elemcount == 0)
			return 0;
		// Create layout
		VertexLayout::Ptr layout = new VertexLayout(elemcount);
		// Set elements
		unsigned int elemidx = 0;
		if (attribs.flags & GeometryFile::AttribFlags::HasPositions)
		{
			layout->setElement(elemidx,
			                   "pos",
			                   0,
			                   3,
			                   attribs.posoffset,
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasNormals)
		{
			layout->setElement(elemidx,
			                   "normal",
			                   0,
			                   3,
			                   attribs.normaloffset,
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasTangents)
		{
			layout->setElement(elemidx,
			                   "tangent",
			                   0,
			                   3,
			                   attribs.tangentoffset,
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasBitangents)
		{
			layout->setElement(elemidx,
			                   "bitangent",
			                   0,
			                   3,
			                   attribs.bitangentoffset,
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		for (unsigned int i = 0; i < attribs.texcoordcount; i++)
		{
			std::ostringstream elemname;
			elemname << "texcoord" << i;
			layout->setElement(elemidx,
			                   elemname.str(),
			                   0,
			                   attribs.texcoordsize[i],
			                   attribs.texcoordoffset[i],
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		for (unsigned int i = 0; i < attribs.colorcount; i++)
		{
			std::ostringstream elemname;
			elemname << "color" << i;
			layout->setElement(elemidx,
			                   elemname.str(),
			                   0,
			                   4,
			                   attribs.coloroffset[i],
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasJoints)
		{
			//getManager()->getLog()->info("%s: Offset: %d/%d.",
			//                              getName().c_str(), attribs.jointweightoffset, attribs.jointoffset);
			layout->setElement(elemidx,
			                   "jointweight",
			                   0,
			                   4,
			                   attribs.jointweightoffset,
			                   VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
			layout->setElement(elemidx,
			                   "jointindex",
			                   0,
			                   4,
			                   attribs.jointoffset,
			                   VertexElementType::Byte,
			                   attribs.stride);
			elemidx++;
		}
		return layout;
	}

	bool Model::waitForLoading(bool recursive, bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		// Wait for the materials
		bool result = true;
		for (unsigned int i = 0; i < meshes.size(); i++)
		{
			if (meshes[i].material)
				result = result && meshes[i].material->waitForLoading(recursive,
				                                                      highpriority);
		}
		return result;
	}

	bool Model::parseNode(TiXmlElement *xml, Model::Node *parent)
	{
		// Get name
		const char *name = xml->Attribute("name");
		if (!name)
		{
			getManager()->getLog()->error("%s: Node name missing.",
			                              getName().c_str());
			return false;
		}
		getManager()->getLog()->info("%s: Node name: %s",
		                              getName().c_str(), name);
		// Add node
		Node *currentnode = addNode(name, parent);
		// Read transformation
		TiXmlElement *transelem = xml->FirstChildElement("Transformation");
		math::Matrix4 transmat = math::Matrix4::Identity();
		if (transelem)
		{
			std::istringstream matstream(transelem->GetText());
			for (unsigned int i = 0; i < 16; i++)
			{
				matstream >> transmat.m[i];
				char separator;
				matstream >> separator;
			}
		}
		currentnode->setTransformation(transmat);
		// Read meshes
		for (TiXmlElement *element = xml->FirstChildElement("Mesh");
		     element != 0;
		     element = element->NextSiblingElement("Mesh"))
		{
			// Read mesh info
			const char *indexstr = element->Attribute("index");
			if (!indexstr)
			{
				getManager()->getLog()->warning("%s: Mesh index missing.",
				                                getName().c_str());
				continue;
			}
			const char *materialfile = element->Attribute("material");
			if (!materialfile)
			{
				getManager()->getLog()->warning("%s: Mesh material missing.",
				                                getName().c_str());
				continue;
			}
			unsigned int batchindex = atoi(indexstr);
			if (batchindex >= batches.size())
			{
				getManager()->getLog()->warning("%s: Invalid batch index %d.",
				                                getName().c_str(),
				                                batchindex);
				continue;
			}
			// Create and add mesh
			std::string path = getPath();
			std::string directory = core::FileSystem::getDirectory(path);
			res::ResourceManager *rmgr = getManager();
			core::FileSystem::Ptr fs = rmgr->getFileSystem();
			Mesh mesh;
			mesh.batch = batchindex;
			std::string materialpath = fs->getPath(materialfile, directory);
			mesh.material = rmgr->getOrLoad<Material>("Material", materialpath);
			mesh.node = currentnode;
			addMesh(mesh);
		}
		// Read child nodes
		for (TiXmlElement *element = xml->FirstChildElement("Node");
		     element != 0;
		     element = element->NextSiblingElement("Node"))
		{
			if (!parseNode(element, currentnode))
				return false;
		}
		return true;
	}
}
}
