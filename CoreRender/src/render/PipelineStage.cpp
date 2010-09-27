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

#include "CoreRender/render/PipelineStage.hpp"
#include "FrameData.hpp"

namespace cr
{
namespace render
{
	PipelineStage::PipelineStage(const std::string &name)
		: name(name)
	{
	}
	PipelineStage::~PipelineStage()
	{
		for (unsigned int i = 0; i < commands.size(); i++)
			delete commands[i];
	}

	unsigned int PipelineStage::addCommand(PipelineCommand *command)
	{
		commands.push_back(command);
		if (command->getType() == PipelineCommandType::BatchList)
			batchlists.push_back((BatchListCommand*)command);
		return commands.size() - 1;
	}
	PipelineCommand *PipelineStage::getCommand(unsigned int index)
	{
		return commands[index];
	}
	void PipelineStage::removeCommand(unsigned int index)
	{
		if (index >= commands.size())
			return;
		if (commands[index]->getType() == PipelineCommandType::BatchList)
		{
			for (unsigned int i = 0; i < batchlists.size(); i++)
			{
				if (batchlists[i] == commands[index])
				{
					batchlists.erase(batchlists.begin() + i);
					break;
				}
			}
		}
		delete commands[index];
		commands.erase(commands.begin() + index);
	}
	unsigned int PipelineStage::getCommandCount()
	{
		return commands.size();
	}

	BatchListCommand *PipelineStage::getBatchList(unsigned int index)
	{
		return batchlists[index];
	}
	unsigned int PipelineStage::getBatchListCount()
	{
		return batchlists.size();
	}

	void PipelineStage::beginFrame(Renderer *renderer,
	                               PipelineSequence *sequence,
	                               PipelineCommandInfo *commands)
	{
		// Apply commands (if possible, batch lists are not complete yet here)
		for (unsigned int i = 0; i < this->commands.size(); i++)
			this->commands[i]->apply(renderer, sequence, &commands[i]);
	}
	void PipelineStage::finish()
	{
		// Insert batch lists
		for (unsigned int i = 0; i < batchlists.size(); i++)
			batchlists[i]->finish();
	}

	std::string PipelineStage::getName()
	{
		return name;
	}
}
}
