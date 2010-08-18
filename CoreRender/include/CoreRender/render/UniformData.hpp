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

#ifndef _CORERENDER_RENDER_UNIFORMDATA_HPP_INCLUDED_
#define _CORERENDER_RENDER_UNIFORMDATA_HPP_INCLUDED_

#include "ShaderVariableType.hpp"
#include "../math/Vector3.hpp"
#include "../math/Vector2.hpp"
#include "../math/Matrix4.hpp"
#include "../render/ShaderVariableType.hpp"

#include <string>
#include <map>

namespace cr
{
namespace render
{
	class Uniform
	{
		public:
			Uniform(const std::string &name);
			Uniform(const std::string &name,
			        ShaderVariableType::List type,
			        float *data);
			Uniform(const Uniform &other);
			~Uniform();

			Uniform &operator=(float f);
			Uniform &operator=(int i);
			Uniform &operator=(const math::Vector2F &v);
			Uniform &operator=(const math::Vector3F &v);
			Uniform &operator=(const math::Vector2I &v);
			Uniform &operator=(const math::Vector3I &v);
			Uniform &operator=(const math::Matrix4 &m);
			Uniform &operator=(const Uniform &other);

			void set(ShaderVariableType::List type,
			         float *data = 0);

			const std::string &getName() const
			{
				return name;
			}

			ShaderVariableType::List getType() const
			{
				return type;
			}
			float *getData() const
			{
				return data;
			}
			void setHandle(int handle)
			{
				shaderhandle = handle;
			}
			int getHandle() const
			{
				return shaderhandle;
			}
		private:
			std::string name;
			ShaderVariableType::List type;
			float *data;
			int shaderhandle;
	};
	class UniformData
	{
		public:
			UniformData();
			UniformData(const UniformData &data);
			~UniformData();

			Uniform &add(const std::string &name);

			void setValues(const UniformData &data);

			UniformData &operator=(const UniformData &data);
			Uniform &operator[](const std::string &name);
			const Uniform &operator[](const std::string &name) const;

			typedef std::map<std::string, Uniform> UniformMap;
			const UniformMap &getData()
			{
				return uniforms;
			}
		private:
			UniformMap uniforms;
			Uniform invalid;
	};
}
}

#endif
