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

#include "CoreRender/render/PipelineDefinition.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	PipelineDefinition::PipelineDefinition(res::ResourceManager *rmgr,
	                                       const std::string &name)
		: Resource(rmgr, name)
	{
	}
	PipelineDefinition::~PipelineDefinition()
	{
	}

	void PipelineDefinition::addDeferredLightLoop(const std::string &listname)
	{
		deferredlightloops.push_back(listname);
	}
	unsigned int PipelineDefinition::getDeferredLightLoopCount()
	{
		return deferredlightloops.size();
	}
	std::string PipelineDefinition::getDeferredLightLoop(unsigned int index)
	{
		if (index >= deferredlightloops.size())
			return 0;
		return deferredlightloops[index];
	}

	void PipelineDefinition::addForwardLightLoop(const std::string &listname)
	{
		forwardlightloops.push_back(listname);
	}
	unsigned int PipelineDefinition::getForwardLightLoopCount()
	{
		return forwardlightloops.size();
	}
	std::string PipelineDefinition::getForwardLightLoop(unsigned int index)
	{
		if (index >= forwardlightloops.size())
			return 0;
		return forwardlightloops[index];
	}

	struct PipelineResources
	{
		std::map<std::string, Texture2D::Ptr> textures;
		std::map<std::string, FrameBuffer::Ptr> framebuffers;
		std::map<std::string, RenderTarget::Ptr> rendertargets;
	};

	Pipeline::Ptr PipelineDefinition::createPipeline(unsigned int screenwidth,
	                                                 unsigned int screenheight)
	{
		Pipeline::Ptr pipeline = new Pipeline();
		PipelineResources res;
		// Create textures
		{
			std::map<std::string, TextureInfo*>::iterator it;
			for (it = this->textures.begin(); it != this->textures.end(); it++)
			{
				float size = it->second->size;
				Texture2D::Ptr tex = getManager()->createResource<Texture2D>("Texture2D");
				tex->set((unsigned int)(size * screenwidth),
				         (unsigned int)(size * screenheight),
				         it->second->format);
				res.textures.insert(std::make_pair(it->first, tex));
			}
		}
		// Create framebuffers
		{
			std::map<std::string, FrameBufferInfo*>::iterator it;
			for (it = this->framebuffers.begin(); it != this->framebuffers.end(); it++)
			{
				float size = it->second->size;
				FrameBuffer::Ptr fb = getManager()->createResource<FrameBuffer>("FrameBuffer");
				fb->setSize((unsigned int)(size * screenwidth),
				            (unsigned int)(size * screenheight),
				            it->second->depthbuffer);
				res.framebuffers.insert(std::make_pair(it->first, fb));
			}
		}
		// Create render targets
		{
			std::map<std::string, RenderTargetInfo*>::iterator it;
			for (it = this->rendertargets.begin(); it != this->rendertargets.end(); it++)
			{
				RenderTarget::Ptr target = getManager()->createResource<RenderTarget>("RenderTarget");
				// TODO: The user might have inserted invalid names here
				target->setFrameBuffer(res.framebuffers[it->second->framebuffer]);
				if (it->second->depthbuffer != "")
					target->setDepthBuffer(res.textures[it->second->depthbuffer]);
				for (unsigned int i = 0; i < it->second->colorbuffers.size(); i++)
				{
					target->addColorBuffer(res.textures[it->second->colorbuffers[i]]);
				}
				res.rendertargets.insert(std::make_pair(it->first, target));
			}
		}
		/*// Create sequences
		for (unsigned int i = 0; i < sequences.size(); i++)
		{
			PipelineSequence *sequence = pipeline->addSequence(sequences[i]->name);
			// Create stages
			for (unsigned int j = 0; j < sequences[i]->stages.size(); j++)
			{
				StageDefinition *src = sequences[i]->stages[j];
				PipelineStage *stage = sequence->addStage(src->name);
				// Create commands
				for (unsigned int k = 0; k < src->commands.size(); k++)
				{
					// TODO: 7 levels of indentation? Too much.
					switch (src->commands[k]->type)
					{
						case PipelineCommandType::SetTarget:
						{
							std::string target = src->commands[k]->params[0];
							SetTargetCommand *cmd = new SetTargetCommand;
							if (target != "")
								cmd->setTarget(rendertargets[target]);
							stage->addCommand(cmd);
							break;
						}
						case PipelineCommandType::BatchList:
						{
							std::string context = src->commands[k]->params[0];
							BatchListCommand *cmd = new BatchListCommand;
							cmd->setContext(context);
							stage->addCommand(cmd);
							break;
						}
						case PipelineCommandType::Clear:
						{
							bool cleardepth = src->commands[k]->params[0] == "true";
							float depth = strtof(src->commands[k]->params[1].c_str(), 0);
							std::istringstream colorstr(src->commands[k]->params[2]);
							float color[4];
							char separator;
							colorstr >> color[0] >> separator;
							colorstr >> color[1] >> separator;
							colorstr >> color[2] >> separator;
							colorstr >> color[3];
							ClearCommand *cmd = new ClearCommand;
							cmd->clearDepth(cleardepth, depth);
							for (unsigned int i = 3; i < src->commands[k]->params.size(); i++)
							{
								unsigned int index = atoi(src->commands[k]->params[i].c_str());
								cmd->clearColor(index, true, core::Color(color[0] * 255.0,
								                                         color[1] * 255.0,
								                                         color[2] * 255.0,
								                                         color[3] * 255.0));
							}
							stage->addCommand(cmd);
							break;
						}
						case PipelineCommandType::Batch:
						{
							std::string materialname = src->commands[k]->params[0];
							std::string context = src->commands[k]->params[1];
							// Get resources needed for the fs quad
							Resource::Ptr res = getManager()->getResource("__fsquadib");
							IndexBuffer::Ptr indices = (IndexBuffer*)res.get();
							res = getManager()->getResource("__fsquadvb");
							VertexBuffer::Ptr vertices = (VertexBuffer*)res.get();
							Material::Ptr material = getManager()->getOrLoad<Material>("Material",
							                                                           materialname);
							// Create vertex layout
							// TODO: We do not want to have one layout instance per quad
							cr::render::VertexLayout::Ptr layout = new cr::render::VertexLayout(2);
							layout->setElement(0, "pos", 0, 2, 0, cr::render::VertexElementType::Float, 16);
							layout->setElement(1, "texcoord0", 0, 2, 8, cr::render::VertexElementType::Float, 16);
							// Create render job
							RenderJob *job = new RenderJob;
							job->vertices = vertices;
							job->indices = indices;
							job->vertexcount = 4;
							job->endindex = 6;
							job->indextype = 1;
							job->startindex = 0;
							job->basevertex = 0;
							job->vertexoffset = 0;
							job->material = material;
							job->layout = layout;
							job->uniforms.add("targetSize") = math::Vector2F(400, 300);
							// Create command
							BatchCommand *cmd = new BatchCommand;
							cmd->setJob(job, context);
							stage->addCommand(cmd);
							break;
						}
						case PipelineCommandType::BindTexture:
						{
							std::string name = src->commands[k]->params[0];
							std::string texturename = src->commands[k]->params[1];
							// Find texture
							std::map<std::string, Texture2D::Ptr>::iterator it;
							it = textures.find(texturename);
							Texture::Ptr texture = 0;
							if (it != textures.end())
								texture = it->second;
							else
							{
								// TODO: Only works for 2D textures
								texture = getManager()->getOrLoad<Texture>("Texture2D", texturename);
							}
							// Create command
							BindTextureCommand *cmd = new BindTextureCommand;
							cmd->setName(name);
							cmd->setTexture(texture);
							stage->addCommand(cmd);
							break;
						}
						case PipelineCommandType::UnbindTextures:
						{
							// Create command
							UnbindTexturesCommand *cmd = new UnbindTexturesCommand;
							stage->addCommand(cmd);
							break;
						}
						default:
							getManager()->getLog()->error("%s: Invalid command.",
							                              getName().c_str());
							break;
					}
				}
			}
			// Copy default sequence
			if (sequences[i]->name == defaultsequence)
				pipeline->setDefaultSequence(sequence);
		}*/
		// Create commands
		createCommands(pipeline, &res, &pipeline->getCommands(), &commands);
		// Copy light loops
		for (unsigned int i = 0; i < forwardlightloops.size(); i++)
		{
			pipeline->addForwardLightLoop(pipeline->getStage(forwardlightloops[i]));
		}
		for (unsigned int i = 0; i < deferredlightloops.size(); i++)
		{
			pipeline->addDeferredLightLoop(pipeline->getStage(forwardlightloops[i]));
		}
		return pipeline;
	}

	void PipelineDefinition::addTexture(const std::string &name,
	                                    TextureFormat::List format,
	                                    float size)
	{
		TextureInfo *texture = new TextureInfo;
		texture->name = name;
		texture->format = format;
		texture->size = size;
		textures.insert(std::make_pair(name, texture));
	}
	PipelineDefinition::TextureInfo *PipelineDefinition::getTexture(const std::string &name)
	{
		std::map<std::string, TextureInfo*>::iterator it = textures.find(name);
		if (it == textures.end())
			return 0;
		return it->second;
	}
	void PipelineDefinition::removeTexture(const std::string &name)
	{
		std::map<std::string, TextureInfo*>::iterator it = textures.find(name);
		if (it == textures.end())
			return;
		textures.erase(it);
	}
	unsigned int PipelineDefinition::getTextureCount()
	{
		return textures.size();
	}

	void PipelineDefinition::addFrameBuffer(const std::string &name,
	                                        float size,
	                                        bool depthbuffer)
	{
		FrameBufferInfo *fb = new FrameBufferInfo;
		fb->name = name;
		fb->size = size;
		fb->depthbuffer = depthbuffer;
		framebuffers.insert(std::make_pair(name, fb));
	}
	PipelineDefinition::FrameBufferInfo *PipelineDefinition::getFrameBuffer(const std::string &name)
	{
		std::map<std::string, FrameBufferInfo*>::iterator it = framebuffers.find(name);
		if (it == framebuffers.end())
			return 0;
		return it->second;
	}
	void PipelineDefinition::removeFrameBuffer(const std::string &name)
	{
		std::map<std::string, FrameBufferInfo*>::iterator it = framebuffers.find(name);
		if (it == framebuffers.end())
			return;
		framebuffers.erase(it);
	}
	unsigned int PipelineDefinition::getFrameBufferCount()
	{
		return framebuffers.size();
	}

	PipelineDefinition::RenderTargetInfo *PipelineDefinition::addRenderTarget(const std::string &name)
	{
		RenderTargetInfo *target = new RenderTargetInfo;
		target->name = name;
		rendertargets.insert(std::make_pair(name, target));
		return target;
	}
	PipelineDefinition::RenderTargetInfo *PipelineDefinition::getRenderTarget(const std::string &name)
	{
		std::map<std::string, RenderTargetInfo*>::iterator it = rendertargets.find(name);
		if (it == rendertargets.end())
			return 0;
		return it->second;
	}
	void PipelineDefinition::removeRenderTarget(const std::string &name)
	{
		std::map<std::string, RenderTargetInfo*>::iterator it = rendertargets.find(name);
		if (it == rendertargets.end())
			return;
		rendertargets.erase(it);
	}
	unsigned int PipelineDefinition::getRenderTargetCount()
	{
		return rendertargets.size();
	}

	bool PipelineDefinition::load()
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
		// Load target textures
		for (TiXmlElement *element = root->FirstChildElement("Texture");
		     element != 0;
		     element = element->NextSiblingElement("Texture"))
		{
			// Get texture name
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->error("%s: Texture name missing.",
				                              getName().c_str());
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
			// Get texture size
			float size = 1.0f;
			const char *sizestr = element->Attribute("size");
			if (sizestr)
			{
				size = strtof(sizestr, 0);
			}
			// Add texture info
			addTexture(name, format, size);
		}
		// Load frame buffers
		for (TiXmlElement *element = root->FirstChildElement("FrameBuffer");
		     element != 0;
		     element = element->NextSiblingElement("FrameBuffer"))
		{
			// Get framebuffer name
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->error("%s: FrameBuffer name missing.",
				                              getName().c_str());
				continue;
			}
			// Get framebuffer size
			float size = 1.0f;
			const char *sizestr = element->Attribute("size");
			if (sizestr)
			{
				size = strtof(sizestr, 0);
			}
			// Get depthbuffer
			bool depthbuffer = false;
			const char *depthbufferstr = element->Attribute("depthbuffer");
			if (depthbufferstr && !strcmp(depthbufferstr, "true"))
			{
				depthbuffer = true;
			}
			// Add framebuffer info
			addFrameBuffer(name, size, depthbuffer);
		}
		// Load render targets
		for (TiXmlElement *element = root->FirstChildElement("RenderTarget");
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
			FrameBufferInfo *fb = getFrameBuffer(fbname);
			if (!fb)
			{
				getManager()->getLog()->error("%s: RenderTarget framebuffer not found.",
				                              getName().c_str());
				continue;
			}
			// Add framebuffer info
			RenderTargetInfo *target = addRenderTarget(name);
			target->framebuffer = fbname;
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
				TextureInfo *texture = getTexture(texname);
				if (!texture)
				{
					getManager()->getLog()->error("%s: DepthBuffer texture not found.",
					                              getName().c_str());
					continue;
				}
				target->depthbuffer = texname;
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
				TextureInfo *texture = getTexture(texname);
				if (!texture)
				{
					getManager()->getLog()->error("%s: ColorBuffer texture not found.",
					                              getName().c_str());
					continue;
				}
				target->colorbuffers.push_back(texname);
			}
		}
		// Load sequences
		TiXmlElement *element = root->FirstChildElement("Commands");
		if (element)
		{
			if (!loadCommands(&commands, element))
			{
				finishLoading(false);
				return false;
			}
		}
		finishLoading(true);
		return true;
	}

	bool PipelineDefinition::loadCommands(CommandDefinition *parent, TiXmlElement *xml)
	{
		// Create command list
		parent->type = PipelineCommandType::CommandList;
		const char *name = xml->Attribute("name");
		if (name)
			parent->params.push_back(name);
		// TODO: Register stage?
		// Read commands
		// TODO: This code is incredibly ugly, replace this with multiple
		// command classes
		for (TiXmlElement *element = xml->FirstChildElement();
		     element != 0;
		     element = element->NextSiblingElement())
		{
			if (!strcmp(element->Value(), "Clear"))
			{
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::Clear;
				const char *cleardepth = element->Attribute("cleardepth");
				if (!cleardepth)
					command->params.push_back("false");
				else
					command->params.push_back(cleardepth);
				const char *depth = element->Attribute("depth");
				if (!depth)
					command->params.push_back("1.0");
				else
					command->params.push_back(depth);
				const char *color = element->Attribute("color");
				if (!color)
					command->params.push_back("0.0, 0.0, 0.0, 1.0");
				else
					command->params.push_back(color);
				for (unsigned int i = 0; i < 16; i++)
				{
					std::ostringstream attribname;
					attribname << "clearcolor" << i;
					const char *clearcolor = element->Attribute(attribname.str().c_str());
					if (clearcolor)
					{
						std::ostringstream index;
						index << i;
						command->params.push_back(index.str());
						command->params.push_back(clearcolor);
					}
				}
				parent->children.push_back(command);
			}
			else if (!strcmp(element->Value(), "SetTarget"))
			{
				// Get target
				const char *name = element->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->error("%s: Target name missing.",
					                              getName().c_str());
					continue;
				}
				if (strcmp(name, "") && !getRenderTarget(name))
				{
					getManager()->getLog()->error("%s: Target \"%s\" not found.",
					                              getName().c_str(),
					                              name);
					continue;
				}
				// Create command
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::SetTarget;
				command->params.push_back(name);
				parent->children.push_back(command);
			}
			else if (!strcmp(element->Value(), "DrawGeometry"))
			{
				// Get context
				const char *context = element->Attribute("context");
				if (!context)
				{
					getManager()->getLog()->error("%s: Context missing.",
					                              getName().c_str());
					continue;
				}
				// Create command
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::BatchList;
				command->params.push_back(context);
				parent->children.push_back(command);
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
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::BindTexture;
				command->params.push_back(name);
				command->params.push_back(texture);
				parent->children.push_back(command);
			}
			else if (!strcmp(element->Value(), "UnbindTextures"))
			{
				// Create command
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::UnbindTextures;
				parent->children.push_back(command);
			}
			else if (!strcmp(element->Value(), "FullscreenQuad"))
			{
				// Get material
				const char *material = element->Attribute("material");
				if (!material)
				{
					getManager()->getLog()->error("%s: Material missing.",
					                              getName().c_str());
					continue;
				}
				// Preload material
				// We have to do this here to be able to wait for loading
				Resource::Ptr materialres = getManager()->getOrLoad("Material",
				                                                    material);
				// Get context
				const char *context = element->Attribute("context");
				if (!context)
				{
					getManager()->getLog()->error("%s: Context missing.",
					                              getName().c_str());
					continue;
				}
				// Create command
				// TODO: We abuse Batch for specific fullscreen quads here
				CommandDefinition *command = new CommandDefinition;
				command->type = PipelineCommandType::Batch;
				command->params.push_back(material);
				command->params.push_back(context);
				command->resources.push_back(materialres);
				parent->children.push_back(command);
			}
			else if (!strcmp(element->Value(), "Stage"))
			{
				CommandDefinition *command = new CommandDefinition;
				parent->children.push_back(command);
				loadCommands(command, element);
			}
			else if (!strcmp(element->Value(), "ForwardLightLoop"))
			{
				// Get name
				const char *name = element->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->error("%s: Light loop name missing.",
					                              getName().c_str());
					continue;
				}
				// Create command list
				CommandDefinition *command = new CommandDefinition;
				parent->children.push_back(command);
				loadCommands(command, element);
				addForwardLightLoop(name);
			}
			else if (!strcmp(element->Value(), "DeferredLightLoop"))
			{
				// Get name
				const char *name = element->Attribute("name");
				if (!name)
				{
					getManager()->getLog()->error("%s: Light loop name missing.",
					                              getName().c_str());
					continue;
				}
				// Create command list
				CommandDefinition *command = new CommandDefinition;
				parent->children.push_back(command);
				loadCommands(command, element);
				addForwardLightLoop(name);
			}
			else
			{
				getManager()->getLog()->error("%s: Invalid command \"%s\".",
				                              getName().c_str(),
				                              element->Value());
			}
		}
		// TODO: Error handling
		return true;
	}

	bool PipelineDefinition::waitForLoading(bool recursive, bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		if (!recursive)
			return true;
		return waitForCommand(&commands, highpriority);
	}

	bool PipelineDefinition::waitForCommand(CommandDefinition *command, bool highpriority)
	{
		bool success = true;
		for (unsigned int i = 0; i < command->resources.size(); i++)
			success = success && command->resources[i]->waitForLoading(true, highpriority);
		for (unsigned int i = 0; i < command->children.size(); i++)
			success = success && waitForCommand(command->children[i], highpriority);
		return success;
	}

	void PipelineDefinition::createCommands(Pipeline::Ptr pipeline,
	                                        PipelineResources *res,
	                                        CommandList *list,
	                                        CommandDefinition *listsrc)
	{
		// Create commands
		for (unsigned int i = 0; i < listsrc->children.size(); i++)
		{
			CommandDefinition *commandsrc = listsrc->children[i];
			switch (commandsrc->type)
			{
				case PipelineCommandType::SetTarget:
				{
					std::string target = commandsrc->params[0];
					SetTargetCommand *cmd = new SetTargetCommand;
					if (target != "")
						cmd->setTarget(res->rendertargets[target]);
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::BatchList:
				{
					std::string context = commandsrc->params[0];
					BatchListCommand *cmd = new BatchListCommand;
					cmd->setContext(context);
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::Clear:
				{
					bool cleardepth = commandsrc->params[0] == "true";
					float depth = strtof(commandsrc->params[1].c_str(), 0);
					std::istringstream colorstr(commandsrc->params[2]);
					float color[4];
					char separator;
					colorstr >> color[0] >> separator;
					colorstr >> color[1] >> separator;
					colorstr >> color[2] >> separator;
					colorstr >> color[3];
					ClearCommand *cmd = new ClearCommand;
					cmd->clearDepth(cleardepth, depth);
					for (unsigned int i = 3; i < commandsrc->params.size(); i++)
					{
						unsigned int index = atoi(commandsrc->params[i].c_str());
						cmd->clearColor(index, true, core::Color(color[0] * 255.0,
						                                         color[1] * 255.0,
						                                         color[2] * 255.0,
						                                         color[3] * 255.0));
					}
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::Batch:
				{
					std::string materialname = commandsrc->params[0];
					std::string context = commandsrc->params[1];
					// Get resources needed for the fs quad
					Resource::Ptr res = getManager()->getResource("__fsquadib");
					IndexBuffer::Ptr indices = (IndexBuffer*)res.get();
					res = getManager()->getResource("__fsquadvb");
					VertexBuffer::Ptr vertices = (VertexBuffer*)res.get();
					Material::Ptr material = getManager()->getOrLoad<Material>("Material",
					                                                           materialname);
					// Create vertex layout
					// TODO: We do not want to have one layout instance per quad
					cr::render::VertexLayout::Ptr layout = new cr::render::VertexLayout(2);
					layout->setElement(0, "pos", 0, 2, 0, cr::render::VertexElementType::Float, 16);
					layout->setElement(1, "texcoord0", 0, 2, 8, cr::render::VertexElementType::Float, 16);
					// Create render job
					RenderJob *job = new RenderJob;
					job->vertices = vertices;
					job->indices = indices;
					job->vertexcount = 4;
					job->endindex = 6;
					job->indextype = 1;
					job->startindex = 0;
					job->basevertex = 0;
					job->vertexoffset = 0;
					job->material = material;
					job->layout = layout;
					job->uniforms.add("targetSize") = math::Vector2F(400, 300);
					// Create command
					BatchCommand *cmd = new BatchCommand;
					cmd->setJob(job, context);
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::BindTexture:
				{
					std::string name = commandsrc->params[0];
					std::string texturename = commandsrc->params[1];
					// Find texture
					std::map<std::string, Texture2D::Ptr>::iterator it;
					it = res->textures.find(texturename);
					Texture::Ptr texture = 0;
					if (it != res->textures.end())
						texture = it->second;
					else
					{
						// TODO: Only works for 2D textures
						texture = getManager()->getOrLoad<Texture>("Texture2D", texturename);
					}
					// Create command
					BindTextureCommand *cmd = new BindTextureCommand;
					cmd->setName(name);
					cmd->setTexture(texture);
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::UnbindTextures:
				{
					// Create command
					UnbindTexturesCommand *cmd = new UnbindTexturesCommand;
					list->addCommand(cmd);
					break;
				}
				case PipelineCommandType::CommandList:
				{
					// Create command
					CommandList *cmd = new CommandList;
					list->addCommand(cmd);
					if (commandsrc->params.size() > 0)
					{
						cmd->setName(commandsrc->params[0]);
						// Add stage to the pipeline
						pipeline->addStage(cmd, commandsrc->params[0]);
					}
					// Recursively add commands
					createCommands(pipeline, res, cmd, commandsrc);
					break;
				}
				default:
					getManager()->getLog()->error("%s: Invalid command.",
					                              getName().c_str());
					break;
			}
		}
	}
}
}
