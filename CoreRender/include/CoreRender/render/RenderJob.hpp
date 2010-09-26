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

#ifndef _CORERENDER_RENDER_RENDERJOB_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERJOB_HPP_INCLUDED_

#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Material.hpp"
#include "VertexLayout.hpp"

namespace cr
{
namespace render
{
	struct RenderBatch;
	class Renderer;

	/**
	 * Defines a combination of geometry/material which can be rendered via
	 * Pipeline::submit().
	 */
	class RenderJob
	{
		public:
			/**
			 * Constructor.
			 */
			RenderJob()
				: startindex(0), endindex(0), basevertex(0), vertexoffset(0),
				indextype(2), sortkey(0.0f)
			{
			}

			/**
			 * Creates the rendering information for this job ready to be sent
			 * to the render thread. Is only called internally, you usually do
			 * not need to call this manually.
			 */
			RenderBatch *createBatch(const std::string &context,
			                         Renderer *renderer,
			                         UniformData &uniforms,
			                         unsigned int flags);

			/**
			 * Vertex buffer resource to be used for rendering.
			 */
			VertexBuffer::Ptr vertices;
			/**
			 * Number of vertices in the vertex buffer. This is needed to
			 * compute vertex offsets for structure-of-array vertex layouts.
			 * @note This can be set to 0 if only array-of-structure-layouts are
			 * used.
			 */
			unsigned int vertexcount;
			/**
			 * Index buffer resource to be used for rendering.
			 */
			IndexBuffer::Ptr indices;
			/**
			 * First index in the index buffer to be rendered. This is not a
			 * byte offset of the index but rather describes how many indices
			 * shall be skipped, e.g. the unit is the index size.
			 */
			unsigned int startindex;
			// TODO: Change this into vertex count?
			/**
			 * First index after the indices to be rendered.
			 */
			unsigned int endindex;
			/**
			 * Vertex offset to be added to all indices. This can be useful to
			 * reduce the index size, e.g. if you want to draw vertices in the
			 * range from 1000..1255 just set this to 1000 and then use indices
			 * in the range 0..255.
			 */
			unsigned int basevertex;
			/**
			 * Byte offset of the vertices in the vertex buffer. This can be
			 * used to pack multiple batches into one vertex buffer.
			 */
			unsigned int vertexoffset;
			/**
			 * Size of a single index in byte. Can be either 1, 2 or 4.
			 */
			unsigned int indextype;
			/**
			 * Material to be used for rendering. Depending on which contexts
			 * are available in the material the job is drawn in certain render
			 * passes.
			 */
			Material::Ptr material;
			/**
			 * Vertex layout of the vertex data to be rendered.
			 */
			VertexLayout::Ptr layout;
			/**
			 * Additional uniforms to be set. This can be used to override the
			 * uniforms set in ShaderText and Material or to provide additional
			 * uniforms.
			 */
			UniformData uniforms;
			/**
			 * Value which is used for sorting all batches in a batch list. This
			 * can be used to sort models from front to back for example.
			 * Jobs with a smaller sort key are rendered first.
			 */
			float sortkey;
		private:
	};
}
}

#endif
