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

#include "MeshOpenGL.hpp"

#include <GL/glew.h>

namespace cr
{
namespace render
{
namespace opengl
{
	MeshOpenGL::MeshOpenGL(UploadManager &uploadmgr, bool usevaobjects)
		: Mesh(uploadmgr), uploaded(0), usevaobjects(usevaobjects), handle(0),
		dirty(false)
	{
	}
	MeshOpenGL::~MeshOpenGL()
	{
		if (uploaded)
			delete uploaded;
		if (handle != 0)
			glDeleteVertexArrays(1, &handle);
	}

	void MeshOpenGL::upload(void *data)
	{
		if (uploaded)
			delete uploaded;
		uploaded = (MeshData*)data;
		dirty = true;
		// Created vertex array object if necessary
		if (usevaobjects)
		{
			if (handle == 0)
			{
				glGenVertexArrays(1, &handle);
			}
		}
	}

	void MeshOpenGL::update()
	{
		dirty = false;
		// TODO: This is not that easy as we have to user-defined attrib
		// locations in the shader?
		// Probably the layout needs to be a shader property
	}
}
}
}

