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

#include "CoreRender/render/PipelineSequence.hpp"
#include "CoreRender/render/Renderable.hpp"
#include "CoreRender/render/RenderJob.hpp"
#include "FrameData.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/render/PipelineStage.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	PipelineSequence::PipelineSequence(const std::string &name)
		: name(name)
	{
	}
	PipelineSequence::~PipelineSequence()
	{
		for (unsigned int i = 0; i < stages.size(); i++)
			delete stages[i];
	}

	PipelineStage *PipelineSequence::addStage(const std::string &name)
	{
		PipelineStage *stage = new PipelineStage(name);
		stages.push_back(stage);
		return stage;
	}
	int PipelineSequence::findStage(const std::string &name)
	{
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			if (stages[i]->getName() == name)
				return i;
		}
		return -1;
	}
	PipelineStage *PipelineSequence::getStage(const std::string &name)
	{
		int index = findStage(name);
		if (index == -1)
			return 0;
		return stages[index];
	}
	PipelineStage *PipelineSequence::getStage(unsigned int index)
	{
		return stages[index];
	}
	void PipelineSequence::removeStage(unsigned int index)
	{
		stages.erase(stages.begin() + index);
	}
	unsigned int PipelineSequence::getStageCount()
	{
		return stages.size();
	}

	void PipelineSequence::submit(Renderable *renderable)
	{
		unsigned int jobcount = renderable->beginRendering();
		// TODO: Reduce these calls to one?
		for (unsigned int i = 0; i < jobcount; i++)
			submit(renderable->getJob(i));
		renderable->endRendering();
	}
	void PipelineSequence::submit(RenderJob *job)
	{
		if (!job->material->getShader())
			return;
		// Get uniform data
		UniformData uniforms = job->material->getShader()->getUniformData();
		uniforms.setValues(job->material->getUniformData());
		uniforms.setValues(job->uniforms);
		// Get flag values
		ShaderText::Ptr text = job->material->getShader();
		unsigned int flags = text->getFlags(job->material->getShaderFlags());
		// Collect batch info
		for (unsigned int i = 0; i < batchlists.size(); i++)
		{
			std::string context = batchlists[i]->getContext();
			Shader::Ptr shader = text->getShader(context, flags);
			if (!shader)
				continue;
			// Allocate render batch
			core::MemoryPool *memory = renderer->getNextFrameMemory();
			RenderBatch *batch = (RenderBatch*)memory->allocate(sizeof(RenderBatch));
			batch->startindex = job->startindex;
			batch->endindex = job->endindex;
			batch->basevertex = job->basevertex;
			batch->vertexoffset = job->vertexoffset;
			batch->indextype = job->indextype;
			batch->vertices = job->vertices->getHandle();
			batch->indices = job->indices->getHandle();
			batch->shader = shader->getHandle();
			batch->blendMode = shader->getBlendMode();
			batch->sortkey = 0.0f; // TODO
			batch->renderflags = 0;
			// Attribs
			if (job->layout->getElementCount() > 0)
			{
				batch->attribcount = job->layout->getElementCount();
				unsigned int memsize = sizeof(AttribMapping) * batch->attribcount;
				batch->attribs = (AttribMapping*)memory->allocate(memsize);
				AttribMapping *attribs = batch->attribs;
				for (unsigned int i = 0; i < batch->attribcount; i++)
				{
					VertexLayoutElement *attrib = job->layout->getElement(i);
					attribs[i].shaderhandle = shader->getAttrib(attrib->name);
					attribs[i].components = attrib->components;
					attribs[i].stride = attrib->stride;
					attribs[i].type = attrib->type;
					attribs[i].address = job->layout->getSlotOffset(job->vertexcount,
					                                                attrib->vbslot)
					                   + attrib->offset;
				}
			}
			else
			{
				batch->attribcount = 0;
				batch->attribs = 0;
			}
			// Uniforms
			const UniformData::UniformMap &uniformdata = uniforms.getData();
			batch->uniformcount = uniformdata.size();
			unsigned int memsize = sizeof(UniformMapping) * batch->uniformcount;
			batch->uniforms = (UniformMapping*)memory->allocate(memsize);
			{
				unsigned int i = 0;
				for (UniformData::UniformMap::const_iterator it = uniformdata.begin();
				     it != uniformdata.end(); ++it, ++i)
				{
					batch->uniforms[i].type = it->second.getType();
					unsigned int size = ShaderVariableType::getSize(batch->uniforms[i].type);
					batch->uniforms[i].shaderhandle = shader->getUniform(it->second.getName());
					batch->uniforms[i].data = (float*)memory->allocate(size * sizeof(float));
					memcpy(batch->uniforms[i].data,
					       it->second.getData(),
					       size * sizeof(float));
				}
			}
			// Textures
			const std::vector<Material::TextureInfo> &textureinfo = job->material->getTextures();
			batch->texcount = textureinfo.size();
			if (textureinfo.size() > 0)
			{
				// TODO: Unify this?
				unsigned int memsize = sizeof(TextureEntry) * batch->texcount;
				TextureEntry *textures = (TextureEntry*)memory->allocate(memsize);
				for (unsigned int i = 0; i < textureinfo.size(); i++)
				{
					textures[i].shaderhandle = shader->getTexture(textureinfo[i].name);
					textures[i].textureindex = i;
					textures[i].texhandle = textureinfo[i].texture->getHandle();
					textures[i].type = textureinfo[i].texture->getTextureType();
				}
				batch->textures = textures;
			}
			else
			{
				batch->textures = 0;
			}
			batchlists[i]->submit(batch);
		}
	}

	void PipelineSequence::beginFrame(Renderer *renderer)
	{
		// Allocate sequence info
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		info = (PipelineSequenceInfo*)memory->allocate(sizeof(PipelineSequenceInfo));
		// Allocate command info data
		info->commandcount = 0;
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			info->commandcount += stages[i]->getCommandCount();
		}
		unsigned int memsize = sizeof(PipelineCommandInfo) * info->commandcount;
		info->commands = (PipelineCommandInfo*)memory->allocate(memsize);
		// Recursively prepare commands/other sequences
		unsigned int commandoffset = 0;
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			stages[i]->beginFrame(renderer, &info->commands[commandoffset]);
			commandoffset += stages[i]->getCommandCount();
		}
		// Collect list of batch lists to insert batches into
		unsigned int batchlistcount = 0;
		for (unsigned int i = 0; i < stages.size(); i++)
			batchlistcount += stages[i]->getBatchListCount();
		batchlists.resize(batchlistcount);
		unsigned int offset = 0;
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			for (unsigned int j = 0; j < stages[i]->getBatchListCount(); j++)
			{
				batchlists[offset + j] = stages[i]->getBatchList(j);
			}
			offset += stages[i]->getBatchListCount();
		}
		// Store renderer, we need this for submit()
		this->renderer = renderer;
	}
	void PipelineSequence::prepare(PipelineInfo *info)
	{
		info->sequence = this->info;
		// Recursively prepare commands/other sequences
		for (unsigned int i = 0; i < stages.size(); i++)
			stages[i]->finish();
	}

	std::string PipelineSequence::getName()
	{
		return name;
	}
}
}
