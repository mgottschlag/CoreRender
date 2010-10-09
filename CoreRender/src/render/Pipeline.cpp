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
#include "FrameData.hpp"

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

	void Pipeline::addStage(CommandList *commands, const std::string &name)
	{
		stages[name] = commands;
	}
	CommandList *Pipeline::getStage(const std::string &name)
	{
		core::HashMap<std::string, CommandList*>::Type::iterator it;
		it = stages.find(name);
		if (it != stages.end())
			return it->second;
		else
			return 0;
	}
	void Pipeline::removeStage(const std::string &name)
	{
		core::HashMap<std::string, CommandList*>::Type::iterator it;
		it = stages.find(name);
		if (it != stages.end())
			stages.erase(it);
	}
	unsigned int Pipeline::getStageCount()
	{
		return stages.size();
	}

	void Pipeline::addDeferredLightLoop(CommandList *lightloop)
	{
		deferredlightloops.push_back(lightloop);
	}
	unsigned int Pipeline::getDeferredLightLoopCount()
	{
		return deferredlightloops.size();
	}
	CommandList *Pipeline::getDeferredLightLoop(unsigned int index)
	{
		if (index >= deferredlightloops.size())
			return 0;
		return deferredlightloops[index];
	}

	void Pipeline::addForwardLightLoop(CommandList *lightloop)
	{
		forwardlightloops.push_back(lightloop);
	}
	unsigned int Pipeline::getForwardLightLoopCount()
	{
		return forwardlightloops.size();
	}
	CommandList *Pipeline::getForwardLightLoop(unsigned int index)
	{
		if (index >= forwardlightloops.size())
			return 0;
		return forwardlightloops[index];
	}

	void Pipeline::getBatchLists(std::vector<BatchListCommand*> &lists)
	{
		getBatchLists(&commands, lists);
	}

	void Pipeline::beginFrame(Renderer *renderer, PipelineInfo *info)
	{
		PipelineState state(uniforms);
		commands.beginFrame(renderer, &state, &info->commands);
	}
	void Pipeline::endFrame()
	{
		commands.endFrame();
	}

	void Pipeline::getBatchLists(CommandList *commands,
	                             std::vector<BatchListCommand*> &lists)
	{
		for (unsigned int i = 0; i < commands->getCommandCount(); i++)
		{
			PipelineCommand *command = commands->getCommand(i);
			if (command->getType() == PipelineCommandType::BatchList)
				lists.push_back((BatchListCommand*)command);
			else if (command->getType() == PipelineCommandType::CommandList)
				getBatchLists((CommandList*)command, lists);
		}
	}
}
}
