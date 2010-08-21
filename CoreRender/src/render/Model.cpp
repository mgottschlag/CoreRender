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

namespace cr
{
namespace render
{
	Model::Model(render::VideoDriver *driver,
	             render::Renderer *renderer,
	             res::ResourceManager *rmgr,
	             const std::string &name)
		: Resource(rmgr, name), driver(driver), renderer(renderer)
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
		// Open file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		core::File::Ptr file = fs->open(path,
		                                core::FileAccess::Read | core::FileAccess::Text);
		if (!file)
		{
			getManager()->getLog()->error("Could not open file \"%s\".",
			                               path.c_str());
			finishLoading(false);
			return false;
		}
		// Open XML file
		unsigned int filesize = file->getSize();
		char *buffer = new char[filesize + 1];
		buffer[filesize] = 0;
		if (file->read(filesize, buffer) != (int)filesize)
		{
			getManager()->getLog()->error("%s: Could not read file content.",
			                              getName().c_str());
			delete[] buffer;
			finishLoading(false);
			return false;
		}
		// Parse XML file
		TiXmlDocument xml(path.c_str());
		xml.Parse(buffer, 0);
		delete[] buffer;
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
		if (!loadGeometryFile(fs->getPath(geofilename, directory)))
		{
			finishLoading(false);
			return false;
		}
		// Get meshes
		for (TiXmlNode *node = root->FirstChild("Mesh");
		     node != 0;
		     node = root->IterateChildren("Mesh", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
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
			res::ResourceManager *rmgr = getManager();
			Mesh mesh;
			mesh.batch = batchindex;
			mesh.material = new Material(driver,
			                             renderer,
			                             rmgr,
			                             rmgr->getInternalName());
			mesh.material->loadFromFile(fs->getPath(materialfile, directory));
			addMesh(mesh);
			getManager()->getLog()->debug("%s: Mesh added: %d.",
				                          getName().c_str(),
				                          batchindex);
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
		vertexbuffer = driver->createVertexBuffer(renderer,
		                                          rmgr,
		                                          rmgr->getInternalName(),
		                                          VertexBufferUsage::Static);
		indexbuffer = driver->createIndexBuffer(renderer,
		                                        rmgr,
		                                        rmgr->getInternalName());
		if (!vertexbuffer || !indexbuffer)
		{
			getManager()->getLog()->error("%s: Could not create buffers.",
			                              getName().c_str());
			free(vertexdata);
			free(indexdata);
			return false;
		}
		vertexbuffer->set(header.vertexdatasize, vertexdata, false);
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
		return layout;
	}
}
}
