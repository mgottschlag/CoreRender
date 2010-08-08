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

#ifndef _CORERENDER_RENDER_SHADERTEXT_HPP_INCLUDED_
#define _CORERENDER_RENDER_SHADERTEXT_HPP_INCLUDED_

#include "../res/Resource.hpp"
#include "Shader.hpp"
#include "ShaderVariableType.hpp"

#include <map>

namespace cr
{
namespace render
{
	class VideoDriver;
	class Renderer;

	class ShaderText : public res::Resource
	{
		public:
			ShaderText(render::VideoDriver *driver,
			           render::Renderer *renderer,
			           res::ResourceManager *rmgr,
			           const std::string &name);
			virtual ~ShaderText();

			bool addText(const std::string &name,
			             const std::string &text,
			             bool autoinclude = false);

			/**
			 * @todo Currently shader creation happens in getShader(). It has
			 * to be done here though as getShader() is called too late.
			 */
			bool addContext(const std::string &name,
			                const std::string &vs,
			                const std::string &fs,
			                const std::string &gs = "",
			                const std::string &ts = "");
			bool hasContext(const std::string &name);

			void addFlag(const std::string &flag, bool defaultvalue);

			void addAttrib(const std::string &name);
			void addUniform(const std::string &name,
			                ShaderVariableType::List type,
			                float *defaultvalue);
			void addTexture(const std::string &name);

			unsigned int getFlags(const std::string &flagsset = "");

			void updateShaders();

			/**
			 * @todo This (and other functions here) needs to be made threadsafe.
			 */
			Shader::Ptr getShader(const std::string &context,
			                      unsigned int flags);

			virtual const char *getType()
			{
				return "ShaderText";
			}

			typedef core::SharedPointer<ShaderText> Ptr;
		private:
			render::VideoDriver *driver;
			render::Renderer *renderer;

			struct Context
			{
				std::string vs;
				std::string fs;
				std::string gs;
				std::string ts;
			};

			struct Uniform
			{
				std::string name;
				ShaderVariableType::List type;
				float *defaultvalue;
			};

			std::map<std::string, std::string> texts;
			std::map<std::string, Context> contexts;

			std::vector<std::string> flags;
			unsigned int flagdefaults;
			std::vector<std::string> attribs;
			std::vector<Uniform> uniforms;
			std::vector<std::string> textures;

			std::vector<Shader::Ptr> shaders;
	};
}
}

#endif
