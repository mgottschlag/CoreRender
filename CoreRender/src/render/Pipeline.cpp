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
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"

#include <cstring>
#include <sstream>

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
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			delete stages[i];
		}
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
	PipelineStage *Pipeline::getStage(unsigned int index)
	{
		if (index >= stages.size())
			return 0;
		return stages[index];
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

	bool Pipeline::load()
	{
		std::string path = getPath();
		std::string directory = core::FileSystem::getDirectory(path);
		// Parse XML file
		TiXmlDocument xml(path.c_str());
		if (!loadResourceFile(xml))
		{
			finishLoading(false);
			return false;
		}
		// Load XML file
		TiXmlNode *root = xml.FirstChild("Pipeline");
		if (!root)
		{
			getManager()->getLog()->error("%s: <Pipeline> not found.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		// Load render target setup
		TiXmlElement *setupelem = root->FirstChildElement("Setup");
		if (setupelem)
		{
			if (!loadSetup(setupelem))
			{
				finishLoading(false);
				return false;
			}
		}
		// Load commands
		TiXmlElement *commandelem = root->FirstChildElement("Commands");
		if (!commandelem)
		{
			getManager()->getLog()->error("%s: <Commands> not found.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		if (!loadCommands(commandelem))
		{
			finishLoading(false);
			return false;
		}
		finishLoading(true);
		return true;
	}

	bool Pipeline::waitForLoading(bool recursive,
	                              bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		if (!recursive)
			return true;
		bool success = true;
		for (unsigned int i = 0; i < stages.size(); i++)
		{
			for (unsigned int j = 0; j < stages[i]->commands.size(); j++)
			{
				if (!stages[i]->commands[j].waitForLoading(recursive, highpriority))
					success = false;
			}
		}
		return success;
	}

	bool Pipeline::loadSetup(TiXmlElement *xml)
	{
		// TODO
		return false;
	}
	bool Pipeline::loadCommands(TiXmlElement *xml)
	{
		for (TiXmlElement *stageelem = xml->FirstChildElement("Stage");
		     stageelem != 0;
		     stageelem = stageelem->NextSiblingElement("Stage"))
		{
			PipelineStage *newstage = new PipelineStage;
			newstage->enabled = true;
			const char *name = stageelem->Attribute("name");
			if (name)
				newstage->name = name;
			if (!loadStage(stageelem, newstage))
			{
				delete newstage;
				return false;
			}
			stages.push_back(newstage);
		}
		return true;
	}
	bool Pipeline::loadStage(TiXmlElement *xml, PipelineStage *stage)
	{
		// Loop through all child elements and search for commands
		for (TiXmlElement *element = xml->FirstChildElement();
		     element != 0;
		     element = element->NextSiblingElement())
		{
			if (!strcmp(element->Value(), "ClearTarget"))
			{
				// Get buffers which shall be cleared
				unsigned int buffers = 0;
				const char *cleardepth = element->Attribute("cleardepth");
				if (cleardepth && !strcmp(cleardepth, "true"))
					buffers |= 0x1;
				// TODO: More buffers (16?)
				const char *clearcolor0 = element->Attribute("clearcolor0");
				if (clearcolor0 && !strcmp(clearcolor0, "true"))
					buffers |= 0x2;
				const char *clearcolor1 = element->Attribute("clearcolor1");
				if (clearcolor1 && !strcmp(clearcolor1, "true"))
					buffers |= 0x4;
				const char *clearcolor2 = element->Attribute("clearcolor2");
				if (clearcolor2 && !strcmp(clearcolor2, "true"))
					buffers |= 0x8;
				const char *clearcolor3 = element->Attribute("clearcolor3");
				if (clearcolor3 && !strcmp(clearcolor3, "true"))
					buffers |= 0x10;
				// Get colors and depth
				float depth = 1.0f;
				const char *depthattrib = element->Attribute("depth");
				if (depthattrib)
					depth = strtof(depthattrib, 0);
				float color[4] = {0.0, 0.0, 0.0, 1.0};
				const char *colorattrib = element->Attribute("color");
				if (colorattrib)
				{
					std::istringstream depthstream(colorattrib);
					char separator;
					depthstream >> color[0] >> separator;
					depthstream >> color[1] >> separator;
					depthstream >> color[2] >> separator;
					depthstream >> color[3];
				}
				// Create command
				PipelineCommand command;
				command.type = PipelineCommandType::ClearTarget;
				command.uintparams.push_back(buffers);
				command.floatparams.push_back(depth);
				for (unsigned int i = 0; i < 4; i++)
					command.floatparams.push_back(color[i]);
				stage->commands.push_back(command);
			}
			else if (!strcmp(element->Value(), "SetTarget"))
			{
			}
			else if (!strcmp(element->Value(), "DrawGeometry"))
			{
				const char *context = element->Attribute("context");
				if (!context)
				{
					getManager()->getLog()->error("%s: DrawGeometry context missing.",
					                              getName().c_str());
					return false;
				}
				PipelineCommand command;
				command.type = PipelineCommandType::DrawGeometry;
				res::NameRegistry &names = getManager()->getNameRegistry();
				command.uintparams.push_back(names.getContext(context));
				stage->commands.push_back(command);
			}
			else if (!strcmp(element->Value(), "BindTexture"))
			{
				// TODO
			}
			else if (!strcmp(element->Value(), "UnbindTextures"))
			{
				// TODO
			}
			else if (!strcmp(element->Value(), "FullscreenQuad"))
			{
				// TODO
			}
			else if (!strcmp(element->Value(), "DoForwardLightLoop"))
			{
				PipelineCommand command;
				command.type = PipelineCommandType::DoForwardLightLoop;
				stage->commands.push_back(command);
			}
			else if (!strcmp(element->Value(), "DoDeferredLightLoop"))
			{
				PipelineCommand command;
				command.type = PipelineCommandType::DoDeferredLightLoop;
				stage->commands.push_back(command);
			}
			else
			{
				getManager()->getLog()->error("%s: Invalid command \"%s\".",
				                              getName().c_str(),
				                              element->Value());
				return false;
			}
		}
		return true;
	}
}
}
