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

#ifndef _CORERENDER_RENDER_SHADER_HPP_INCLUDED_
#define _CORERENDER_RENDER_SHADER_HPP_INCLUDED_

#include "RenderResource.hpp"
#include <map>

namespace cr
{
namespace render
{
	class ShaderText;

	class Shader : public RenderResource
	{
		public:
			Shader(Renderer *renderer,
			       res::ResourceManager *rmgr,
			       const std::string &name);
			virtual ~Shader();

			void setVertexShader(const std::string &vs);
			void setFragmentShader(const std::string &fs);
			void setGeometryShader(const std::string &gs);
			void setTesselationShader(const std::string &ts);

			void addAttrib(const std::string &name);
			void addUniform(const std::string &name);
			int getAttrib(const std::string &name);
			int getUniform(const std::string &name);

			void updateShader();

			virtual void uploadShader()
			{
			}

			int getHandle()
			{
				return handle;
			}
			virtual const char *getType()
			{
				return "Shader";
			}

			void setShaderText(ShaderText *text)
			{
				this->text = text;
			}
			ShaderText *getShaderText()
			{
				return text;
			}

			typedef core::SharedPointer<Shader> Ptr;
		protected:
			int handle;
			int oldhandle;

			ShaderText *text;

			tbb::spin_mutex textmutex;
			std::string vs;
			std::string fs;
			std::string gs;
			std::string ts;

			std::map<std::string, int> attribs;
			std::map<std::string, int> uniforms;
	};
}
}

#endif
