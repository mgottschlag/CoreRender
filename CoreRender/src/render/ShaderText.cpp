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
			return false;
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
		Uniform uniform = {
			name, type, defaultvalue
		};
		uniforms.push_back(uniform);
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
		for (unsigned int i = 0; i < uniforms.size(); i++)
			shader->addUniform(uniforms[i].name);
		// Finish shader
		shader->setShaderText(this);
		shader->updateShader();
		shaders.push_back(shader);
		return shader;
	}
}
}
