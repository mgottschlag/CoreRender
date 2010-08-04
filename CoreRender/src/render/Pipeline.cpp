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

#include "CoreRender/render/Pipeline.hpp"
#include "CoreRender/render/RenderBatch.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	Pipeline::Pipeline()
	{
	}
	Pipeline::~Pipeline()
	{
	}

	void Pipeline::addPass(RenderPass::Ptr pass)
	{
		passes.push_back(pass);
	}
	void Pipeline::removePass(RenderPass::Ptr pass)
	{
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			if (passes[i] == pass)
			{
				passes.erase(passes.begin() + i);
				return;
			}
		}
	}
	void Pipeline::removePass(unsigned int index)
	{
		if (index >= passes.size())
			return;
		passes.erase(passes.begin() + index);
	}
	RenderPass::Ptr Pipeline::getPass(unsigned int index)
	{
		return passes[index];
	}
	unsigned int Pipeline::getPassCount()
	{
		return passes.size();
	}

	void Pipeline::submit(RenderJob *job)
	{
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			std::string context = passes[i]->getContext();
			ShaderText::Ptr text = job->material->getShader();
			Shader::Ptr shader = text->getShader(context, text->getFlags(""));
			if (!shader)
				continue;
			// TODO: Memory leak, use the memory pool here
			RenderBatch *batch = new RenderBatch;
			// TODO: This memset should not be necessary
			memset(batch, 0, sizeof(RenderBatch));
			batch->startindex = job->startindex;
			batch->endindex = job->endindex;
			batch->basevertex = job->basevertex;
			batch->vertices = job->vertices->getHandle();
			batch->indices = job->indices->getHandle();
			batch->shader = shader->getHandle();
			batch->renderflags = 0;
			batch->uniformcount = 0;
			batch->attribcount = 0;
			batch->texcount = 0;
			batch->texmappingcount = 0;
			passes[i]->insert(batch);
		}
	}

	void Pipeline::beginFrame()
	{
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			passes[i]->beginFrame();
		}
	}
	void Pipeline::prepare(PipelineInfo *info)
	{
		info->passcount = passes.size();
		info->passes = new RenderPassInfo[passes.size()];
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			passes[i]->prepare(&info->passes[i]);
		}
	}
	void Pipeline::render(VideoDriver *driver)
	{
		for (unsigned int i = 0; i < passes.size(); i++)
		{
			passes[i]->render(driver);
		}
	}
}
}
