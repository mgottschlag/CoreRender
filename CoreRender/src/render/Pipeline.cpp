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
#include "CoreRender/render/PipelineSequence.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	Pipeline::Pipeline()
		: defaultsequence(0)
	{
	}
	Pipeline::~Pipeline()
	{
		for (unsigned int i = 0; i < sequences.size(); i++)
			delete sequences[i];
	}

	PipelineSequence *Pipeline::addSequence(const std::string &name)
	{
		PipelineSequence *sequence = new PipelineSequence(name);
		sequences.push_back(sequence);
		return sequence;
	}
	int Pipeline::findSequence(const std::string &name)
	{
		for (unsigned int i = 0; i < sequences.size(); i++)
		{
			if (sequences[i]->getName() == name)
				return i;
		}
		return -1;
	}
	PipelineSequence *Pipeline::getSequence(const std::string &name)
	{
		for (unsigned int i = 0; i < sequences.size(); i++)
		{
			if (sequences[i]->getName() == name)
				return sequences[i];
		}
		return 0;
	}
	PipelineSequence *Pipeline::getSequence(unsigned int index)
	{
		if (index >= sequences.size())
			return 0;
		return sequences[index];
	}
	void Pipeline::removeSequence(unsigned int index)
	{
		if (index >= sequences.size())
			return;
		delete sequences[index];
		sequences.erase(sequences.begin() + index);
	}
	unsigned int Pipeline::getSequenceCount()
	{
		return sequences.size();
	}

	void Pipeline::setDefaultSequence(PipelineSequence *sequence)
	{
		defaultsequence = sequence;
	}
	PipelineSequence *Pipeline::getDefaultSequence()
	{
		return defaultsequence;
	}

	void Pipeline::addDeferredLightLoop(PipelineSequence *sequence)
	{
		deferredlightloops.push_back(sequence);
	}
	unsigned int Pipeline::getDeferredLightLoopCount()
	{
		return deferredlightloops.size();
	}
	PipelineSequence *Pipeline::getDeferredLightLoop(unsigned int index)
	{
		if (index >= deferredlightloops.size())
			return 0;
		return deferredlightloops[index];
	}

	void Pipeline::addForwardLightLoop(PipelineSequence *sequence)
	{
		forwardlightloops.push_back(sequence);
	}
	unsigned int Pipeline::getForwardLightLoopCount()
	{
		return forwardlightloops.size();
	}
	PipelineSequence *Pipeline::getForwardLightLoop(unsigned int index)
	{
		if (index >= forwardlightloops.size())
			return 0;
		return forwardlightloops[index];
	}

	void Pipeline::beginFrame(Renderer *renderer)
	{
		if (!defaultsequence)
			return;
		defaultsequence->beginFrame(renderer);
	}
	void Pipeline::prepare(PipelineInfo *info)
	{
		if (!defaultsequence)
		{
			info->sequence = 0;
			return;
		}
		defaultsequence->prepare(info);
	}
}
}
