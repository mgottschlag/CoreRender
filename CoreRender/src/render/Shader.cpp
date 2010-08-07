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

#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/Renderer.hpp"

namespace cr
{
namespace render
{
	Shader::Shader(Renderer *renderer,
	               res::ResourceManager *rmgr,
	               const std::string &name)
		: RenderResource(renderer, rmgr, name), handle(0), oldhandle(0), text(0)
	{
	}
	Shader::~Shader()
	{
	}

	void Shader::setVertexShader(const std::string &vs)
	{
		tbb::spin_mutex::scoped_lock lock(textmutex);
		this->vs = vs;
	}
	void Shader::setFragmentShader(const std::string &fs)
	{
		tbb::spin_mutex::scoped_lock lock(textmutex);
		this->fs = fs;
	}
	void Shader::setGeometryShader(const std::string &gs)
	{
		tbb::spin_mutex::scoped_lock lock(textmutex);
		this->gs = gs;
	}
	void Shader::setTesselationShader(const std::string &ts)
	{
		tbb::spin_mutex::scoped_lock lock(textmutex);
		this->ts = ts;
	}

	void Shader::addAttrib(const std::string &name)
	{
		attribs.insert(std::make_pair(name, -1));
	}
	void Shader::addUniform(const std::string &name)
	{
		uniforms.insert(std::make_pair(name, -1));
	}
	int Shader::getAttrib(const std::string &name)
	{
		std::map<std::string, int>::iterator it = attribs.find(name);
		if (it == attribs.end())
			return -1;
		return it->second;
	}
	int Shader::getUniform(const std::string &name)
	{
		std::map<std::string, int>::iterator it = uniforms.find(name);
		if (it == uniforms.end())
			return -1;
		return it->second;
	}

	void Shader::updateShader()
	{
		getRenderer()->registerShaderUpload(this);
	}
}
}
