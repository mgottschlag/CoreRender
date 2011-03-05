/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "CoreRender/render/Mesh.hpp"

namespace cr
{
namespace render
{
	Mesh::Mesh(UploadManager &uploadmgr)
		: RenderObject(uploadmgr), layoutchangecounter(0), indextype(1),
		startindex(0), indexcount(0), basevertex(0), vertexoffset(0),
		vertexcount(0), primitivetype(PrimitiveType::Triangle)
	{
	}
	Mesh::~Mesh()
	{
	}

	void Mesh::setVertexBuffer(VertexBuffer::Ptr vertices)
	{
		this->vertices = vertices;
		registerUpload();
	}

	void Mesh::setIndexBuffer(IndexBuffer::Ptr indices)
	{
		this->indices = indices;
		registerUpload();
	}

	void Mesh::setVertexLayout(VertexLayout::Ptr layout)
	{
		this->layout = layout;
		layoutchangecounter = layout->getChangeCounter();
		registerUpload();
	}

	void Mesh::setIndexType(unsigned int indextype)
	{
		this->indextype = indextype;
		registerUpload();
	}

	void Mesh::setStartIndex(unsigned int startindex)
	{
		this->startindex = startindex;
		registerUpload();
	}

	void Mesh::setIndexCount(unsigned int indexcount)
	{
		this->indexcount = indexcount;
		registerUpload();
	}

	void Mesh::setBaseVertex(unsigned int basevertex)
	{
		this->basevertex = basevertex;
		registerUpload();
	}

	void Mesh::setVertexOffset(unsigned int vertexoffset)
	{
		this->vertexoffset = vertexoffset;
		registerUpload();
	}

	void Mesh::setVertexCount(unsigned int vertexcount)
	{
		this->vertexcount = vertexcount;
		registerUpload();
	}

	void Mesh::setPrimitiveType(PrimitiveType::List type)
	{
		primitivetype = type;
		registerUpload();
	}

	void Mesh::checkForUpdate()
	{
		// Check whether the layout was modified
		if (layout->getChangeCounter() != layoutchangecounter)
			registerUpload();
	}

	void *Mesh::getUploadData()
	{
		MeshData *data = new MeshData();
		data->vertices = vertices.get();
		data->indices = indices.get();
		data->layout = layout.get();
		data->layoutchangecounter = layoutchangecounter;
		data->indextype = indextype;
		data->startindex = startindex;
		data->indexcount = indexcount;
		data->basevertex = basevertex;
		data->vertexoffset = vertexoffset;
		data->vertexcount = vertexcount;
		data->primitivetype = primitivetype;
		return data;
	}
}
}
