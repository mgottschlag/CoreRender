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

#include "CoreRender/render/PipelineCommand.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/render/RenderJob.hpp"
#include "CoreRender/render/Pipeline.hpp"
#include "CoreRender/render/Renderable.hpp"
#include "FrameData.hpp"

#include <cstring>
#include <tbb/parallel_sort.h>

namespace cr
{
namespace render
{
	void ClearCommand::beginFrame(Renderer *renderer,
	                              PipelineState *state,
	                              PipelineCommandInfo *command)
	{
		// Allocate clear info
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		ClearInfo *info = (ClearInfo*)memory->allocate(sizeof(ClearInfo));
		info->depth = depth;
		info->depthbuffer = cleardepth;
		info->colorbuffercount = color.size();
		unsigned int memsize = color.size() * sizeof(ColorBufferClearInfo);
		info->colorbuffers = (ColorBufferClearInfo*)memory->allocate(memsize);
		for (unsigned int i = 0; i < color.size(); i++)
		{
			info->colorbuffers[i].index = color[i].index;
			info->colorbuffers[i].color[0] = color[i].color[0];
			info->colorbuffers[i].color[1] = color[i].color[1];
			info->colorbuffers[i].color[2] = color[i].color[2];
			info->colorbuffers[i].color[3] = color[i].color[3];
		}
		// Fill in command
		command->type = getType();
		command->clear = info;
	}

	void SetTargetCommand::beginFrame(Renderer *renderer,
	                                  PipelineState *state,
	                                  PipelineCommandInfo *command)
	{
		if (!target || !target->getFrameBuffer())
		{
			command->target = 0;
			command->type = getType();
			return;
		}
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		command->target = (RenderTargetInfo*)memory->allocate(sizeof(RenderTargetInfo));
		FrameBuffer::Ptr framebuffer = target->getFrameBuffer();
		command->target->framebuffer = &framebuffer->getConfiguration();
		command->target->width = framebuffer->getWidth();
		command->target->height = framebuffer->getHeight();
		// Fill in depth buffer info
		if (target->getDepthBuffer())
			command->target->depthbuffer = target->getDepthBuffer()->getHandle();
		else
			command->target->depthbuffer = 0;
		// Fill in color buffer info
		unsigned int colorbuffercount = target->getColorBufferCount();
		command->target->colorbuffercount = colorbuffercount;
		unsigned int memsize = sizeof(unsigned int) * colorbuffercount;
		command->target->colorbuffers = (unsigned int*)memory->allocate(memsize);
		for (unsigned int i = 0; i < colorbuffercount; i++)
		{
			command->target->colorbuffers[i] = target->getColorBuffer(i)->getHandle();
		}
		// Fill in general command info
		command->type = getType();
	}

	void BindTextureCommand::beginFrame(Renderer *renderer,
	                                    PipelineState *state,
	                                    PipelineCommandInfo *command)
	{
		if (name == "")
			return;
		if (texture)
			state->textures[name] = texture;
		else
		{
			std::map<std::string, Texture::Ptr>::iterator it;
			it = state->textures.find(name);
			if (it != state->textures.end())
				state->textures.erase(it);
		}
		// Fill in command
		command->type = getType();
	}
	void UnbindTexturesCommand::beginFrame(Renderer *renderer,
	                                       PipelineState *state,
	                                       PipelineCommandInfo *command)
	{
		state->textures.clear();
		// Fill in command
		command->type = getType();
	}

	void BatchListCommand::submit(Renderable *renderable)
	{
		unsigned int jobcount = renderable->beginRendering();
		// TODO: Reduce these calls to one?
		for (unsigned int i = 0; i < jobcount; i++)
			submit(renderable->getJob(i));
		renderable->endRendering();
	}
	void BatchListCommand::submit(RenderJob *job)
	{
		if (!job->material->getShader())
			return;
		// TODO: Check the context earlier!
		// Get uniform data
		UniformData uniforms = job->material->getShader()->getUniformData();
		uniforms.setValues(job->material->getUniformData());
		uniforms.setValues(job->uniforms);
		uniforms.setValues(getUniformData());
		// Get default uniform data
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		DefaultUniformValues *defuniforms;
		defuniforms = (DefaultUniformValues*)memory->allocate(sizeof(DefaultUniformValues));
		for (unsigned int i = 0; i < pipelinestate->defuniforms.size(); i++)
		{
			memcpy(defuniforms->uniforms[pipelinestate->defuniforms[i].name],
			       pipelinestate->defuniforms[i].data, 16 * sizeof(float));
		}
		for (unsigned int i = 0; i < defuniform.size(); i++)
		{
			memcpy(defuniforms->uniforms[defuniform[i].name],
			       defuniform[i].data, 16 * sizeof(float));
		}
		for (unsigned int i = 0; i < job->defaultuniforms.size(); i++)
		{
			memcpy(defuniforms->uniforms[job->defaultuniforms[i].name],
			       job->defaultuniforms[i].data, 16 * sizeof(float));
		}
		// Get flag values
		ShaderText::Ptr text = job->material->getShader();
		unsigned int flags = text->getFlags(job->material->getShaderFlags());
		// Collect batch info
		RenderBatch *batch = job->createBatch(getContext(),
		                                      renderer,
		                                      pipelinestate,
		                                      uniforms,
		                                      defuniforms,
		                                      flags);
		if (batch)
			submit(batch);
	}

	void BatchListCommand::submit(RenderBatch *batch)
	{
		batches.push_back(batch);
	}

	void BatchListCommand::beginFrame(Renderer *renderer,
	                                  PipelineState *state,
	                                  PipelineCommandInfo *command)
	{
		command->type = getType();
		info = command;
		this->renderer = renderer;
		pipelinestate = new PipelineState(*state);
	}

	struct BatchListLess
	{
		bool operator()(RenderBatch *a, RenderBatch *b) const
		{
			return a->sortkey < b->sortkey;
		}
	};
	void BatchListCommand::endFrame()
	{
		// Sort batches
		if (batches.size() > 0)
			tbb::parallel_sort(batches.begin(),
			                   batches.begin() + (batches.size() - 1),
			                   BatchListLess());
		// Allocate memory for the batches
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		info->batchlist = (BatchList*)memory->allocate(sizeof(BatchList));
		unsigned int memsize = batches.size() * sizeof(RenderBatch*);
		info->batchlist->batchcount = batches.size();
		info->batchlist->batches = (RenderBatch**)memory->allocate(memsize);
		// Copy batches
		memcpy(info->batchlist->batches, &batches[0], memsize);
		// Clear temporary batch list
		batches.clear();
		delete pipelinestate;
	}

	BatchCommand::BatchCommand()
		: PipelineCommand(PipelineCommandType::Batch), job(0)
	{
	}
	BatchCommand::~BatchCommand()
	{
		if (job)
			delete job;
	}

	void BatchCommand::setJob(RenderJob *job, const std::string &context)
	{
		if (this->job)
			delete this->job;
		this->job = job;
		this->context = context;
	}

	void BatchCommand::beginFrame(Renderer *renderer,
	                              PipelineState *state,
	                              PipelineCommandInfo *command)
	{
		command->type = getType();
		if (!job->material->getShader())
		{
			command->batch = 0;
			return;
		}
		// Get uniform data
		UniformData uniforms = job->material->getShader()->getUniformData();
		uniforms.setValues(job->material->getUniformData());
		uniforms.setValues(job->uniforms);
		// Get default uniform data
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		DefaultUniformValues *defuniforms;
		defuniforms = (DefaultUniformValues*)memory->allocate(sizeof(DefaultUniformValues));
		for (unsigned int i = 0; i < state->defuniforms.size(); i++)
		{
			memcpy(defuniforms->uniforms[state->defuniforms[i].name],
			       state->defuniforms[i].data, 16 * sizeof(float));
		}
		for (unsigned int i = 0; i < job->defaultuniforms.size(); i++)
		{
			memcpy(defuniforms->uniforms[job->defaultuniforms[i].name],
			       job->defaultuniforms[i].data, 16 * sizeof(float));
		}
		// Get flag values
		ShaderText::Ptr text = job->material->getShader();
		unsigned int flags = text->getFlags(job->material->getShaderFlags());
		// Create batch
		command->batch = job->createBatch(context,
		                                  renderer,
		                                  state,
		                                  uniforms,
		                                  defuniforms,
		                                  flags);
	}

	CommandList::CommandList()
		: PipelineCommand(PipelineCommandType::CommandList), active(true)
	{
	}
	CommandList::~CommandList()
	{
		for (unsigned int i = 0; i < commands.size(); i++)
			delete commands[i];
	}

	void CommandList::setActive(bool active)
	{
		this->active = active;
	}
	bool CommandList::isActive()
	{
		return active;
	}

	void CommandList::addCommand(PipelineCommand *command)
	{
		commands.push_back(command);
	}
	PipelineCommand *CommandList::getCommand(unsigned int index)
	{
		if (index >= commands.size())
			return 0;
		return commands[index];
	}
	void CommandList::removeCommand(unsigned int index)
	{
		if (index >= commands.size())
			return;
		delete commands[index];
		commands.erase(commands.begin() + index);
	}
	unsigned int CommandList::getCommandCount()
	{
		return commands.size();
	}

	void CommandList::beginFrame(Renderer *renderer,
	                             PipelineState *state,
	                             PipelineCommandInfo *command)
	{
		command->type = getType();
		// Allocate memory for children
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		PipelineCommandInfo *children;
		unsigned int memsize = sizeof(PipelineCommandInfo) * commands.size();
		children = (PipelineCommandInfo*)memory->allocate(memsize);
		command->list.commands = children;
		command->list.size = commands.size();
		// Prepare children
		for (unsigned int i = 0; i < commands.size(); i++)
		{
			commands[i]->beginFrame(renderer, state, &children[i]);
		}
	}
	void CommandList::endFrame()
	{
		for (unsigned int i = 0; i < commands.size(); i++)
		{
			commands[i]->endFrame();
		}
	}
}
}
