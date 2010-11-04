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

#include <cstring>

namespace cr
{
namespace render
{
	Pipeline::Pipeline(res::ResourceManager *rmgr, const std::string &name)
		: Resource(rmgr, name)
	{
	}
	Pipeline::~Pipeline()
	{
	}

	PipelineStage *Pipeline::addStage(const std::string &name)
	{
		PipelineStage *newstage = new PipelineStage;
		newstage->name = name;
		stages.push_back(newstage);
		return newstage;
	}
	PipelineStage *Pipeline::getStage(const std::string &name)
	{
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			if (stages[i]->name == name)
				return stages[i];
		}
		return 0;
	}
	void Pipeline::removeStage(const std::string &name)
	{
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			if (stages[i]->name == name)
			{
				delete stages[i];
				stages.erase(stages.begin() + i);
				return;
			}
		}
	}
	unsigned int Pipeline::getStageCount()
	{
		return stages.size();
	}
}
}
