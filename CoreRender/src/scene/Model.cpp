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

#include "CoreRender/scene/Model.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/GraphicsEngine.hpp"

#include <sstream>

namespace cr
{
namespace scene
{
	Model::Model(GraphicsEngine *graphics,
	             const std::string &name)
		: Resource(graphics->getResourceManager(), name), graphics(graphics), changecounter(0)
	{
	}
	Model::~Model()
	{
	}

	void Model::render(render::RenderQueue &queue,
	                  math::Mat4f transmat)
	{
		// TODO: Culling
		for (unsigned int i = 0; i < batches.size(); i++)
		{
			// Create batch
			render::Batch *batch = prepareBatch(queue, i, false, false);
			if (!batch)
				continue;
			batch->transmat = transmat * nodes[batches[i].node].abstrans;
			// Add batch to the render queue
			tbb::mutex::scoped_lock lock(queue.batchmutex);
			queue.batches.push_back(batch);
		}
	}
	void Model::render(render::RenderQueue &queue,
	                  unsigned int instancecount,
	                  math::Mat4f *transmat)
	{
		core::MemoryPool *memory = queue.memory;
		// Create transformation matrix list
		unsigned int memsize = sizeof(math::Mat4f) * instancecount;
		math::Mat4f *matrices = (math::Mat4f*)memory->allocate(memsize);
		for (unsigned int i = 0; i < instancecount; i++)
			matrices[i] = transmat[i];
		// Create batches
		// TODO: Culling
		for (unsigned int i = 0; i < batches.size(); i++)
		{
			// Create batch
			render::Batch *batch = prepareBatch(queue, i, true, false);
			if (!batch)
				continue;
			batch->transmat = math::Mat4f::Identity();
			batch->transmatcount = instancecount;
			batch->transmatlist = matrices;
			// Add batch to the render queue
			tbb::mutex::scoped_lock lock(queue.batchmutex);
			queue.batches.push_back(batch);
		}
	}

	render::Batch *Model::prepareBatch(render::RenderQueue &queue,
	                                  unsigned int batchindex,
	                                  bool instancing,
	                                  bool skinning)
	{
		Batch &meshbatch = batches[batchindex];
		if (!meshbatch.material)
			return 0;
		if (meshbatch.geometry >= geometry.size())
			return 0;
		if (meshbatch.node >= nodes.size())
			return 0;
		return queue.prepareBatch(meshbatch.material.get(),
		                          geometry[meshbatch.geometry].mesh.get(),
		                          instancing,
		                          skinning);
		/*core::MemoryPool *memory = queue.memory;
		// Get material information
		render::Material::Ptr material = meshbatch.material;
		if (!material->getShader())
			return 0;
		unsigned int shaderflagmask;
		unsigned int shaderflagvalue;
		material->getShaderFlags(shaderflagmask,
		                         shaderflagvalue);
		render::ShaderCombination *shader;
		shader = material->getShader()->getCombination(queue.context,
		                                               shaderflagmask,
		                                               shaderflagvalue,
		                                               instancing,
		                                               skinning).get();
		if (!shader)
			return 0;
		BatchGeometry *geom = &geometry[meshbatch.geometry];
		// Create batch
		void *ptr = memory->allocate(sizeof(render::Batch));
		render::Batch *batch = (render::Batch*)ptr;
		batch->mesh = geom->mesh.get();
		batch->shader = shader;
		batch->material = material.get();
		batch->transmatcount = 0;
		batch->skinmat = 0;
		batch->skinmatcount = 0;
		// TODO: Sorting
		batch->sortkey = 0.0f;
		// Custom uniforms
		const std::vector<render::Material::UniformInfo> &uniforms = material->getUniforms();
		batch->customuniformcount = uniforms.size();
		if (uniforms.size() > 0)
		{
			ptr = memory->allocate(sizeof(render::CustomUniform) * uniforms.size());
			render::CustomUniform *uniformdata = (render::CustomUniform*)ptr;
			for (unsigned int i = 0; i < uniforms.size(); i++)
			{
				uniformdata[i].size = uniforms[i].size;
				uniformdata[i].data = (float*)memory->allocate(sizeof(float) * uniforms[i].size);
				memcpy(uniformdata[i].data,
				       uniforms[i].data,
				       uniforms[i].size * sizeof(float));
				uniformdata[i].name = (char*)memory->allocate(uniforms[i].name.length() + 1);
				strcpy(uniformdata[i].name, uniforms[i].name.c_str());
			}
			batch->customuniforms = uniformdata;
		}
		else
			batch->customuniforms = 0;
		return batch;*/
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
		if (!parseNode(rootnodeelem, -1))
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
			const char *geomstr = element->Attribute("batch");
			if (!geomstr)
			{
				getManager()->getLog()->warning("%s: Armature batch missing.",
				                                getName().c_str());
				continue;
			}
			unsigned int geomidx = atoi(geomstr);
			if (geomidx >= geometry.size())
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
				if (index >= geometry[geomidx].joints.size())
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
				int nodeidx = getNode(name);
				if (nodeidx == -1)
				{
					getManager()->getLog()->warning("%s: Joint node \"%s\" not found.",
					                                getName().c_str(), name);
					continue;
				}
				geometry[geomidx].joints[index].node = nodeidx;
			}
		}
		changecounter++;
		finishLoading(true);
		return true;
	}

	bool Model::waitForLoading(bool recursive,
	                          bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		if (!recursive)
			return true;
		bool success = true;
		for (unsigned int i = 0; i < batches.size(); i++)
		{
			if (!batches[i].material->waitForLoading(recursive, highpriority))
				success = false;
		}
		return success;
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
		// Read the global bounding box
		// TODO: The resulting box is way too large if multiple models are
		// packed into one model file
		boundingbox.minCorner.x = header.boundingbox[0];
		boundingbox.minCorner.y = header.boundingbox[1];
		boundingbox.minCorner.z = header.boundingbox[2];
		boundingbox.maxCorner.x = header.boundingbox[3];
		boundingbox.maxCorner.y = header.boundingbox[4];
		boundingbox.maxCorner.z = header.boundingbox[5];
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
		vertices = rmgr->createResource<render::VertexBuffer>("VertexBuffer");
		indices = rmgr->createResource<render::IndexBuffer>("IndexBuffer");
		if (!vertices || !indices)
		{
			getManager()->getLog()->error("%s: Could not create buffers.",
			                              getName().c_str());
			free(vertexdata);
			free(indexdata);
			return false;
		}
		vertices->set(header.vertexdatasize,
		              vertexdata,
		              render::VertexBufferUsage::Static,
		              false);
		indices->set(header.indexdatasize,
		             indexdata,
		             render::IndexBufferUsage::Static,
		             false);
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
				getManager()->getLog()->error("%s: Could not read joint matrices.",
				                              getName().c_str());
				return false;
			}
		}
		// Construct geometry batch info
		geometry.resize(header.batchcount);
		for (unsigned int i = 0; i < header.batchcount; i++)
		{
			geometry[i].mesh = graphics->createMesh();
			render::VertexLayout::Ptr layout = createVertexLayout(batchdata[i].attribs);
			if (!layout)
			{
				getManager()->getLog()->error("%s: Could not create vertex layout.",
											  getName().c_str());
				return false;
			}
			geometry[i].mesh->setVertexLayout(layout);
			GeometryFile::AttribInfo &attribs = batchdata[i].attribs;
			GeometryFile::GeometryInfo &geom = batchdata[i].geom;
			geometry[i].mesh->setBaseVertex(geom.basevertex);
			geometry[i].mesh->setIndexType(geom.indextype);
			geometry[i].mesh->setIndexCount(geom.indexcount);
			geometry[i].mesh->setStartIndex(geom.indexoffset / geom.indextype);
			geometry[i].mesh->setVertexOffset(geom.vertexoffset);
			geometry[i].mesh->setVertexCount(geom.vertexsize / attribs.stride);
			geometry[i].mesh->setVertexBuffer(vertices);
			geometry[i].mesh->setIndexBuffer(indices);
			// Create joints
			geometry[i].joints.resize(batchdata[i].geom.jointcount);
			for (unsigned int j = 0; j < batchdata[i].geom.jointcount; j++)
			{
				math::Mat4f &jointmat = geometry[i].joints[j].jointmat;
				memcpy(&jointmat.m[0],
				       &batchdata[i].jointmatrices[j * 16],
				       sizeof(float) * 16);
				geometry[i].joints[j].node = -1;
			}
		}
		return true;
	}
	render::VertexLayout::Ptr Model::createVertexLayout(const GeometryFile::AttribInfo &attribs)
	{
		if (attribs.texcoordcount > GeometryFile::maxtexcoords)
			return 0;
		if (attribs.colorcount > GeometryFile::maxcolors)
			return 0;
		res::NameRegistry &names = getManager()->getNameRegistry();
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
		render::VertexLayout::Ptr layout = new render::VertexLayout(getManager()->getUploadManager(),
		                                                            elemcount);
		// Set elements
		unsigned int elemidx = 0;
		if (attribs.flags & GeometryFile::AttribFlags::HasPositions)
		{
			layout->setElement(elemidx,
			                   names.getAttrib("pos"),
			                   0,
			                   3,
			                   attribs.posoffset,
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasNormals)
		{
			layout->setElement(elemidx,
			                   names.getAttrib("normal"),
			                   0,
			                   3,
			                   attribs.normaloffset,
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasTangents)
		{
			layout->setElement(elemidx,
			                   names.getAttrib("tangent"),
			                   0,
			                   3,
			                   attribs.tangentoffset,
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasBitangents)
		{
			layout->setElement(elemidx,
			                   names.getAttrib("bitangent"),
			                   0,
			                   3,
			                   attribs.bitangentoffset,
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		for (unsigned int i = 0; i < attribs.texcoordcount; i++)
		{
			std::ostringstream elemname;
			elemname << "texcoord" << i;
			layout->setElement(elemidx,
			                   names.getAttrib(elemname.str()),
			                   0,
			                   attribs.texcoordsize[i],
			                   attribs.texcoordoffset[i],
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		for (unsigned int i = 0; i < attribs.colorcount; i++)
		{
			std::ostringstream elemname;
			elemname << "color" << i;
			layout->setElement(elemidx,
			                   names.getAttrib(elemname.str()),
			                   0,
			                   4,
			                   attribs.coloroffset[i],
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
		}
		if (attribs.flags & GeometryFile::AttribFlags::HasJoints)
		{
			layout->setElement(elemidx,
			                   names.getAttrib("jointweight"),
			                   0,
			                   4,
			                   attribs.jointweightoffset,
			                   render::VertexElementType::Float,
			                   attribs.stride);
			elemidx++;
			layout->setElement(elemidx,
			                   names.getAttrib("jointindex"),
			                   0,
			                   4,
			                   attribs.jointoffset,
			                   render::VertexElementType::UnsignedByte,
			                   attribs.stride);
			elemidx++;
		}
		return layout;
	}

	bool Model::parseNode(TiXmlElement *xml, int parent)
	{
		// Get name
		const char *name = xml->Attribute("name");
		if (!name)
		{
			getManager()->getLog()->error("%s: Node name missing.",
			                              getName().c_str());
			return false;
		}
		Node newnode;
		newnode.name = name;
		// Read transformation
		TiXmlElement *transelem = xml->FirstChildElement("Transformation");
		math::Mat4f transmat = math::Mat4f::Identity();
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
		newnode.transmat = transmat;
		newnode.parent = parent;
		if (parent == -1)
			// Root node
			newnode.abstrans = transmat;
		else
			newnode.abstrans = transmat * nodes[parent].transmat;
		newnode.abstransinverse = newnode.abstrans.inverse();
		// Read batches
		for (TiXmlElement *element = xml->FirstChildElement("Mesh");
		     element != 0;
		     element = element->NextSiblingElement("Mesh"))
		{
			// Read batch info
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
			unsigned int geometryindex = atoi(indexstr);
			if (geometryindex >= geometry.size())
			{
				getManager()->getLog()->warning("%s: Invalid batch index %d.",
				                                getName().c_str(),
				                                geometryindex);
				continue;
			}
			// Create and add batch
			std::string path = getPath();
			std::string directory = core::FileSystem::getDirectory(path);
			res::ResourceManager *rmgr = getManager();
			core::FileSystem::Ptr fs = rmgr->getFileSystem();
			Batch batch;
			batch.geometry = geometryindex;
			std::string materialpath = fs->getPath(materialfile, directory);
			batch.material = rmgr->getOrLoad<render::Material>("Material",
			                                                   materialpath);
			batch.node = nodes.size();
			batches.push_back(batch);
		}
		// Add new node to node list
		unsigned int nodeindex = nodes.size();
		nodes.push_back(newnode);
		// Read child nodes
		for (TiXmlElement *element = xml->FirstChildElement("Node");
		     element != 0;
		     element = element->NextSiblingElement("Node"))
		{
			if (!parseNode(element, nodeindex))
				return false;
		}
		return true;
	}
}
}
