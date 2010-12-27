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
#include <CoreRender/render/Material.hpp>

namespace cr
{
namespace render
{
	Pipeline::Pipeline(res::ResourceManager *rmgr, const std::string &name)
		: Resource(rmgr, name)
	{
		targetsize[0] = 512;
		targetsize[1] = 512;
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

	void Pipeline::resizeTargets(unsigned int width, unsigned int height)
	{
		for (unsigned int i = 0; i < framebuffers.size(); i++)
		{
			framebuffers[i].fb->setSize(width,
			                            height,
			                            framebuffers[i].fb->hasDepthBuffer());
		}
		for (unsigned int i = 0; i < targettextures.size(); i++)
		{
			targettextures[i].texture->set2D(width,
			                                 height,
			                                 targettextures[i].texture->getInternalFormat());
		}
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
		// Load textures
		for (TiXmlElement *element = xml->FirstChildElement("Texture");
		     element != 0;
		     element = element->NextSiblingElement("Texture"))
		{
			// Get texture name
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->error("%s: Texture name missing.",
				                              getName().c_str());
				continue;
			}
			// Get texture format
			const char *formatstr = element->Attribute("format");
			TextureFormat::List format = TextureFormat::RGBA8;
			if (formatstr)
			{
				format = TextureFormat::fromString(formatstr);
				if (format == TextureFormat::Invalid)
				{
					getManager()->getLog()->error("%s: Invalid texture format \"%s\".",
					                              getName().c_str(),
					                              formatstr);
					continue;
				}
			}
			TargetTextureInfo texture;
			texture.name = name;
			parseSize(element, texture.relsize, texture.abssize);
			// Create texture resource
			unsigned int texturesize[2];
			texturesize[0] = (unsigned int)(texture.relsize[0] * targetsize[0])
			               + texture.abssize[0];
			texturesize[1] = (unsigned int)(texture.relsize[1] * targetsize[1])
			               + texture.abssize[1];
			Texture::Ptr texres = getManager()->createResource<Texture>("Texture");
			texres->set2D(texturesize[0], texturesize[1], format);
			texres->setMipmapsEnabled(false);
			// TODO: Configurable filtering
			texres->setFiltering(TextureFiltering::Nearest);
			texture.texture = texres;
			// Add the texture to the texture list
			targettextures.push_back(texture);
		}
		// Load frame buffer resources
		for (TiXmlElement *element = xml->FirstChildElement("FrameBuffer");
		     element != 0;
		     element = element->NextSiblingElement("FrameBuffer"))
		{
			// Get texture name
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->error("%s: Texture name missing.",
				                              getName().c_str());
				continue;
			}
			FrameBufferInfo fb;
			fb.name = name;
			parseSize(element, fb.relsize, fb.abssize);
			// Get depthbuffer
			bool depthbuffer = false;
			const char *depthbufferstr = element->Attribute("depthbuffer");
			if (depthbufferstr && !strcmp(depthbufferstr, "true"))
			{
				depthbuffer = true;
			}
			// Create framebuffer
			fb.fb = getManager()->createResource<FrameBuffer>("FrameBuffer");
			unsigned int fbsize[2];
			fbsize[0] = (unsigned int)(fb.relsize[0] * targetsize[0])
			          + fb.abssize[0];
			fbsize[1] = (unsigned int)(fb.relsize[1] * targetsize[1])
			          + fb.abssize[1];
			fb.fb->setSize(fbsize[0], fbsize[1], depthbuffer);
			framebuffers.push_back(fb);
		}
		// Load render targets
		for (TiXmlElement *element = xml->FirstChildElement("RenderTarget");
		     element != 0;
		     element = element->NextSiblingElement("RenderTarget"))
		{
			// Get target name
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->error("%s: RenderTarget name missing.",
				                              getName().c_str());
				continue;
			}
			// Get framebuffer
			const char *fbname = element->Attribute("framebuffer");
			if (!fbname)
			{
				getManager()->getLog()->error("%s: RenderTarget framebuffer name missing.",
				                              getName().c_str());
				continue;
			}
			FrameBuffer::Ptr fb = getFrameBuffer(fbname);
			if (!fb)
			{
				getManager()->getLog()->error("%s: RenderTarget framebuffer not found.",
				                              getName().c_str());
				continue;
			}
			// Create render target
			RenderTargetInfo target;
			target.target = getManager()->createResource<RenderTarget>("RenderTarget");
			target.name = name;
			target.target->setFrameBuffer(fb);
			// Depth buffer
			TiXmlElement *depthbufferelem = element->FirstChildElement("DepthBuffer");
			if (depthbufferelem)
			{
				const char *texname = depthbufferelem->Attribute("texture");
				if (!texname)
				{
					getManager()->getLog()->error("%s: DepthBuffer texture missing.",
					                              getName().c_str());
					continue;
				}
				Texture::Ptr texture = getTargetTexture(texname);
				if (!texture)
				{
					getManager()->getLog()->error("%s: DepthBuffer texture not found.",
					                              getName().c_str());
					continue;
				}
				target.target->setDepthBuffer(texture);
			}
			// Color buffers
			for (TiXmlElement *colorbufferelem = element->FirstChildElement("ColorBuffer");
			     colorbufferelem != 0;
			     colorbufferelem = colorbufferelem->NextSiblingElement("ColorBuffer"))
			{
				const char *texname = colorbufferelem->Attribute("texture");
				if (!texname)
				{
					getManager()->getLog()->error("%s: ColorBuffer texture missing.",
					                              getName().c_str());
					continue;
				}
				Texture::Ptr texture = getTargetTexture(texname);
				if (!texture)
				{
					getManager()->getLog()->error("%s: ColorBuffer texture not found.",
					                              getName().c_str());
					continue;
				}
				target.target->addColorBuffer(texture);
			}
			rendertargets.push_back(target);
		}
		return true;
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
					depth = (float)strtod(depthattrib, 0);
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
				const char *name = element->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->error("%s: Target name missing.",
					                              getName().c_str());
					continue;
				}
				PipelineCommand command;
				command.type = PipelineCommandType::SetTarget;
				if (strcmp(name, ""))
				{
					RenderTarget::Ptr target = getRenderTarget(name);
					if (!target)
					{
						getManager()->getLog()->error("%s: Target \"%s\" not found.",
						                              getName().c_str(),
						                              name);
						continue;
					}
					command.resources.push_back(target);
				}
				else
				{
					command.resources.push_back(0);
				}
				stage->commands.push_back(command);
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
				// Get sampler name
				const char *name = element->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->error("%s: Sampler name missing.",
					                              getName().c_str());
					continue;
				}
				// Get texture name
				const char *texture = element->Attribute("texture");
				if (!texture)
				{
					getManager()->getLog()->error("%s: Texture name missing.",
					                              getName().c_str());
					continue;
				}
				// Create command
				PipelineCommand command;
				command.type = PipelineCommandType::BindTexture;
				command.stringparams.push_back(name);
				if (strcmp(texture, ""))
				{
					Texture::Ptr textureres = getTargetTexture(texture);
					if (!textureres)
					{
						getManager()->getLog()->error("%s: Texture \"%s\" not found.",
						                              getName().c_str(),
						                              texture);
						continue;
					}
					command.resources.push_back(textureres);
				}
				else
				{
					command.resources.push_back(0);
				}
				stage->commands.push_back(command);
			}
			else if (!strcmp(element->Value(), "UnbindTextures"))
			{
				PipelineCommand command;
				command.type = PipelineCommandType::UnbindTextures;
				stage->commands.push_back(command);
			}
			else if (!strcmp(element->Value(), "FullscreenQuad"))
			{
				const char *contextstr = element->Attribute("context");
				if (!contextstr)
				{
					getManager()->getLog()->error("%s: Context name missing.",
					                              getName().c_str());
					continue;
				}
				const char *materialname = element->Attribute("material");
				if (!materialname)
				{
					getManager()->getLog()->error("%s: Material name missing.",
					                              getName().c_str());
					continue;
				}
				PipelineCommand command;
				command.type = PipelineCommandType::DrawFullscreenQuad;
				unsigned int context = getManager()->getNameRegistry().getContext(contextstr);
				command.uintparams.push_back(context);
				Material::Ptr material;
				material = getManager()->getOrLoad<Material>("Material",
				                                             materialname);
				command.resources.push_back(material);
				stage->commands.push_back(command);
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

	void Pipeline::parseSize(TiXmlElement *xml, float *relsize, int *abssize)
	{
		abssize[0] = 0;
		abssize[1] = 0;
		relsize[0] = 1.0;
		relsize[1] = 1.0;
		// TODO: Allow mixing relative and absolute size?
		const char *relsizestr = xml->Attribute("relsize");
		const char *abssizestr = xml->Attribute("abssize");
		if (relsizestr)
		{
			// The user can omit one component if both are the same
			if (strchr(relsizestr,','))
			{
				std::istringstream stream(relsizestr);
				char separator;
				stream >> relsize[0] >> separator >> relsize[1];
			}
			else
			{
				relsize[0] = (float)strtod(relsizestr, 0);
				relsize[1] = relsize[0];
			}
		}
		else if(abssizestr)
		{
			relsize[0] = 0;
			relsize[1] = 0;
			// The user can omit one component if both are the same
			if (strchr(abssizestr,','))
			{
				std::istringstream stream(abssizestr);
				char separator;
				stream >> abssize[0] >> separator >> abssize[1];
			}
			else
			{
				abssize[0] = atoi(abssizestr);
				abssize[1] = abssize[0];
			}
		}
	}

	FrameBuffer::Ptr Pipeline::getFrameBuffer(const std::string &name)
	{
		for (unsigned int i = 0; i < framebuffers.size(); i++)
		{
			if (framebuffers[i].name == name)
				return framebuffers[i].fb;
		}
		return 0;
	}
	Texture::Ptr Pipeline::getTargetTexture(const std::string &name)
	{
		for (unsigned int i = 0; i < targettextures.size(); i++)
		{
			if (targettextures[i].name == name)
				return targettextures[i].texture;
		}
		return 0;
	}
	RenderTarget::Ptr Pipeline::getRenderTarget(const std::string &name)
	{
		for (unsigned int i = 0; i < rendertargets.size(); i++)
		{
			if (rendertargets[i].name == name)
				return rendertargets[i].target;
		}
		return 0;
	}
}
}
