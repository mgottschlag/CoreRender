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

#include "CoreRender/render/ShaderText.hpp"
#include "CoreRender/render/VideoDriver.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	ShaderText::ShaderText(render::VideoDriver *driver,
	                       render::Renderer *renderer,
	                       res::ResourceManager *rmgr,
	                       const std::string &name)
		: Resource(rmgr, name), driver(driver), renderer(renderer),
		flagdefaults(0)
	{
	}
	ShaderText::~ShaderText()
	{
	}

	bool ShaderText::addText(const std::string &name,
	                         const std::string &text,
	                         bool autoinclude)
	{
		if (autoinclude)
		{
			// TODO
		}
		texts[name] = text;
		return true;
	}

	bool ShaderText::addContext(const std::string &name,
	                            const std::string &vs,
	                            const std::string &fs,
	                            const std::string &gs,
	                            const std::string &ts)
	{
		Context context = {
			vs, fs, gs, ts
		};
		// Store context info
		contexts[name] = context;
		return true;
	}

	void ShaderText::addFlag(const std::string &flag, bool defaultvalue)
	{
		unsigned int index = flags.size();
		if (index == 32)
		{
			// TODO: Warning
			return;
		}
		flags.push_back(flag);
		if (defaultvalue)
			flagdefaults |= (1 << index);
	}

	void ShaderText::addAttrib(const std::string &name)
	{
		attribs.push_back(name);
	}
	void ShaderText::addUniform(const std::string &name,
	                            ShaderVariableType::List type,
	                            float *defaultvalue)
	{
		/*Uniform uniform = {
			name, type, defaultvalue
		};
		uniforms.push_back(uniform);*/
		uniforms.add(name).set(type, defaultvalue);
	}
	void ShaderText::addTexture(const std::string &name)
	{
		textures.push_back(name);
	}

	unsigned int ShaderText::getFlags(const std::string &flagsset)
	{
		return 0;
	}

	void ShaderText::updateShaders()
	{
	}

	Shader::Ptr ShaderText::getShader(const std::string &context,
	                                  unsigned int flags)
	{
		// Get shader name
		std::ostringstream shadername;
		shadername << "__" << getName() << "_Shader" << context << "_" << flags;
		// Look whether the shader already exists
		for (unsigned int i = 0; i < shaders.size(); i++)
		{
			if (shaders[i]->getName() == shadername.str())
				return shaders[i];
		}
		// Get context info
		std::map<std::string, Context>::iterator it;
		it = contexts.find(context);
		if (it == contexts.end())
			return 0;
		Context &ctx = it->second;
		// TODO: Set flags
		// Create shader
		// TODO: Check whether this name already is taken
		Shader::Ptr shader = driver->createShader(renderer,
		                                          getManager(),
		                                          shadername.str());
		// Check whether texts exists
		if (texts.find(ctx.vs) == texts.end())
		{
			renderer->getLog()->error("Text \"%s\" not found.", ctx.vs.c_str());
			return false;
		}
		if (texts.find(ctx.fs) == texts.end())
		{
			renderer->getLog()->error("Text \"%s\" not found.", ctx.fs.c_str());
			return false;
		}
		if (ctx.gs != "" && texts.find(ctx.gs) == texts.end())
		{
			renderer->getLog()->error("Text \"%s\" not found.", ctx.gs.c_str());
			return false;
		}
		if (ctx.ts != "" && texts.find(ctx.ts) == texts.end())
		{
			renderer->getLog()->error("Text \"%s\" not found.", ctx.ts.c_str());
			return false;
		}
		// Set shader data
		shader->setVertexShader(texts[ctx.vs]);
		shader->setFragmentShader(texts[ctx.fs]);
		if (ctx.gs != "")
			shader->setGeometryShader(texts[ctx.gs]);
		if (ctx.ts != "")
			shader->setTesselationShader(texts[ctx.ts]);
		// Add attribs
		for (unsigned int i = 0; i < attribs.size(); i++)
			shader->addAttrib(attribs[i]);
		// Add uniforms
		{
			UniformData::UniformMap::const_iterator it;
			for (it = uniforms.getData().begin();
			     it != uniforms.getData().end();
			     it++)
			{
				shader->addUniform(it->second.getName());
			}
		}
		// Add textures
		for (unsigned int i = 0; i < textures.size(); i++)
			shader->addTexture(textures[i]);
		// Finish shader
		shader->setShaderText(this);
		shader->updateShader();
		shaders.push_back(shader);
		return shader;
	}

	bool ShaderText::load()
	{
		std::string path = getPath();
		// Open file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		core::File::Ptr file = fs->open(path,
		                                core::FileAccess::Read | core::FileAccess::Text);
		if (!file)
		{
			getManager()->getLog()->error("Could not open file \"%s\".",
			                               path.c_str());
			finishLoading(false);
			return false;
		}
		// Open XML file
		unsigned int filesize = file->getSize();
		char *buffer = new char[filesize + 1];
		buffer[filesize] = 0;
		if (file->read(filesize, buffer) != (int)filesize)
		{
			getManager()->getLog()->error("%s: Could not read file content.",
			                              getName().c_str());
			delete[] buffer;
			finishLoading(false);
			return false;
		}
		// Parse XML file
		TiXmlDocument xml(path.c_str());
		xml.Parse(buffer, 0);
		delete[] buffer;
		// Load XML file
		TiXmlNode *root = xml.FirstChild("Shader");
		if (!root)
		{
			getManager()->getLog()->error("%s: <Shader> not found.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		// Load shader texts
		for (TiXmlNode *node = root->FirstChild("Text");
		     node != 0;
		     node = root->IterateChildren("Text", node))
		{
			getManager()->getLog()->debug("%s: Text.",
			                              getName().c_str());
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read text
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Shader text name missing.",
				                                getName().c_str());
				continue;
			}
			const char *content = element->GetText();
			if (!content)
			{
				getManager()->getLog()->warning("%s: Shader text empty.",
				                                getName().c_str());
				continue;
			}
			getManager()->getLog()->debug("%s: Adding text %s.",
			                                getName().c_str(), name);
			addText(name, content, true);
		}
		// Load contexts
		for (TiXmlNode *node = root->FirstChild("Context");
		     node != 0;
		     node = root->IterateChildren("Context", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read context info
			const char *name = element->Attribute("name");
			const char *vsname = element->Attribute("vs");
			const char *fsname = element->Attribute("fs");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Context name missing.",
				                                getName().c_str());
				continue;
			}
			if (!fsname || !vsname)
			{
				getManager()->getLog()->warning("%s: Not FS or VS given for context %s.",
				                                getName().c_str(), name);
				continue;
			}
			const char *gsnameattrib = element->Attribute("gs");
			std::string gsname = "";
			if (gsnameattrib)
				gsname = gsnameattrib;
			const char *tsnameattrib = element->Attribute("ts");
			std::string tsname = "";
			if (tsnameattrib)
				tsname = tsnameattrib;
			// Add context
			addContext(name, vsname, fsname, gsname, tsname);
		}
		// Add attribs
		for (TiXmlNode *node = root->FirstChild("Attrib");
		     node != 0;
		     node = root->IterateChildren("Attrib", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read text
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Attrib name missing.",
				                                getName().c_str());
				continue;
			}
			// Add attrib
			addAttrib(name);
		}
		// Add uniforms
		for (TiXmlNode *node = root->FirstChild("Uniform");
		     node != 0;
		     node = root->IterateChildren("Uniform", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read unform info
			const char *name = element->Attribute("name");
			const char *typestr = element->Attribute("type");
			if (!name || !typestr)
			{
				getManager()->getLog()->warning("%s: Uniform declaration invalid!",
				                                getName().c_str());
				continue;
			}
			// Get uniform type
			ShaderVariableType::List type;
			if (!strcmp(typestr, "float1"))
			{
				type = ShaderVariableType::Float;
			}
			else if (!strcmp(typestr, "float2"))
			{
				type = ShaderVariableType::Float2;
			}
			else if (!strcmp(typestr, "float3"))
			{
				type = ShaderVariableType::Float3;
			}
			else if (!strcmp(typestr, "float4"))
			{
				type = ShaderVariableType::Float4;
			}
			else if (!strcmp(typestr, "mat3"))
			{
				type = ShaderVariableType::Float3x3;
			}
			else if (!strcmp(typestr, "mat3x4"))
			{
				type = ShaderVariableType::Float3x4;
			}
			else if (!strcmp(typestr, "mat4x3"))
			{
				type = ShaderVariableType::Float4x3;
			}
			else if (!strcmp(typestr, "mat4"))
			{
				type = ShaderVariableType::Float4x4;
			}
			else
			{
				getManager()->getLog()->warning("%s: Unknown uniform type for %s.",
				                                getName().c_str(), name);
				continue;
			}
			// Get uniform default value
			unsigned int size = ShaderVariableType::getSize(type);
			float *defdata = new float[size];
			const char *content = element->GetText();
			if (!content)
			{
				memset(defdata, 0, sizeof(float) * size);
			}
			else
			{
				std::istringstream stream(content);
				for (unsigned int i = 0; i < size; i++)
				{
					stream >> defdata[i];
					getManager()->getLog()->warning("%s: Read: %f.",
					                                getName().c_str(), defdata[i]);
					char separator;
					stream >> separator;
				}
			}
			// Add uniform
			addUniform(name, type, defdata);
			delete[] defdata;
		}
		// Add textures
		for (TiXmlNode *node = root->FirstChild("Texture");
		     node != 0;
		     node = root->IterateChildren("Texture", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read text
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Texture name missing.",
				                                getName().c_str());
				continue;
			}
			// Add texture
			addTexture(name);
		}
		// Add flags
		// TODO
		finishLoading(true);
		return true;
	}
}
}
