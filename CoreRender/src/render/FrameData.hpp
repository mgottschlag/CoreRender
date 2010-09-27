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

#ifndef _CORERENDER_RENDER_FRAMEDATA_HPP_INCLUDED_
#define _CORERENDER_RENDER_FRAMEDATA_HPP_INCLUDED_

#include "CoreRender/render/VertexLayout.hpp"
#include "CoreRender/render/ShaderVariableType.hpp"
#include "CoreRender/render/FrameBuffer.hpp"
#include "CoreRender/render/BlendMode.hpp"
#include "CoreRender/render/PipelineCommand.hpp"

namespace cr
{
namespace render
{
	struct TextureEntry
	{
		int texhandle;
		int type;
		int shaderhandle;
		int textureindex;
	};
	struct UniformMapping
	{
		int shaderhandle;
		ShaderVariableType::List type;
		float *data;
	};
	struct AttribMapping
	{
		int shaderhandle;
		unsigned int address;
		unsigned int stride;
		unsigned int components;
		VertexElementType::List type;
	};
	/**
	 * Raw optimized batch data to be passed to the render thread.
	 */
	struct RenderBatch
	{
		TextureEntry *textures;
		UniformMapping *uniforms;
		AttribMapping *attribs;

		unsigned int texcount;
		unsigned int uniformcount;
		unsigned int attribcount;

		int shader;
		int vertices;
		int indices;

		const DefaultUniformLocations *defuniformlocations;
		DefaultUniformValues *defuniforms;

		float sortkey;
		unsigned int startindex;
		unsigned int endindex;
		unsigned int basevertex;
		unsigned int vertexoffset;
		unsigned int indextype;

		BlendMode::List blendmode;
		bool depthwrite;
		DepthTest::List depthtest;
		unsigned int renderflags;
	};


	struct ColorBufferClearInfo
	{
		unsigned int index;
		float color[4];
	};
	struct ClearInfo
	{
		bool depthbuffer;
		float depth;
		unsigned int colorbuffercount;
		ColorBufferClearInfo *colorbuffers;
	};

	struct RenderTargetInfo
	{
		unsigned int width;
		unsigned int height;
		FrameBuffer::Configuration *framebuffer;
		unsigned int depthbuffer;
		unsigned int colorbuffercount;
		unsigned int *colorbuffers;
	};

	struct BatchList
	{
		unsigned int batchcount;
		RenderBatch **batches;
	};

	struct PipelineSequenceInfo;

	struct PipelineCommandInfo
	{
		PipelineCommandType::List type;
		union
		{
			RenderBatch *batch;
			BatchList *batchlist;
			ClearInfo *clear;
			PipelineSequenceInfo *sequence;
			RenderTargetInfo *target;
		};
	};

	struct PipelineSequenceInfo
	{
		unsigned int commandcount;
		PipelineCommandInfo *commands;
	};

	struct PipelineInfo
	{
		PipelineSequenceInfo *sequence;
	};
}
}

#endif
