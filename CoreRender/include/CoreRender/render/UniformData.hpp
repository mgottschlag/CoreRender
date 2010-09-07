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
#include "../core/HashMap.hpp"

#include <string>

namespace cr
{
namespace render
{
	/**
	 * Class contain the value/type of a single shader uniform.
	 */
	class Uniform
	{
		public:
			/**
			 * Constructor.
			 * @param name Name of the uniform in shaders.
			 */
			Uniform(const std::string &name);
			/**
			 * Constructor.
			 * @param name Name of the uniform in shaders.
			 * @param type Type of the uniform variable.
			 * @param data Value of the uniform.
			 */
			Uniform(const std::string &name,
			        ShaderVariableType::List type,
			        float *data);
			/**
			 * Copy constructor.
			 * @param other Uniform to copy all data from.
			 */
			Uniform(const Uniform &other);
			/**
			 * Destructor.
			 */
			~Uniform();

			Uniform &operator=(float f);
			Uniform &operator=(int i);
			Uniform &operator=(const math::Vector2F &v);
			Uniform &operator=(const math::Vector3F &v);
			Uniform &operator=(const math::Vector2I &v);
			Uniform &operator=(const math::Vector3I &v);
			Uniform &operator=(const math::Matrix4 &m);
			Uniform &operator=(const Uniform &other);

			/**
			 * Sets uniform type and value.
			 * @param type New type.
			 * @param data New uniform data. If this is 0, the value will be
			 * initialized with zeroes.
			 */
			void set(ShaderVariableType::List type,
			         float *data = 0);

			/**
			 * Returns the name of the uniform.
			 * @return Name.
			 */
			const std::string &getName() const
			{
				return name;
			}

			/**
			 * Returns the type of the uniform variable in shaders.
			 * @return Type.
			 */
			ShaderVariableType::List getType() const
			{
				return type;
			}
			/**
			 * Returns the value of the uniform. The length of the returned
			 * buffer depends on the type.
			 * @return Pointer to the value of the uniform.
			 */
			const float *getData() const
			{
				return data;
			}
			/**
			 * Returns the value of the uniform. The length of the returned
			 * buffer depends on the type.
			 * @return Pointer to the value of the uniform.
			 */
			float *getData()
			{
				return data;
			}
			/**
			 * Sets the shader handle of this uniform.
			 * @param handle Handle of the uniform (as returned by
			 * glGetUniformLocation()).
			 */
			void setHandle(int handle)
			{
				shaderhandle = handle;
			}
			/**
			 * Returns the shader handle of this uniform.
			 * @return Shader handle.
			 */
			int getHandle() const
			{
				return shaderhandle;
			}
		private:
			std::string name;
			ShaderVariableType::List type;
			float data[16];
			int shaderhandle;
	};
	/**
	 * Class defining types and values of a set of uniforms.
	 *
	 * Usually you have 3 instances of this class defining the uniform values
	 * for a certain RenderJob: First there is the default data from ShaderText
	 * which is then overridden by the values contained in the data set from
	 * Material which in turn can be overridden by the user in RenderJob.
	 */
	class UniformData
	{
		public:
			/**
			 * Constructor.
			 */
			UniformData();
			/**
			 * Copy constructor.
			 * @param data Data to copy all uniforms from.
			 */
			UniformData(const UniformData &data);
			/**
			 * Destructor.
			 */
			~UniformData();

			/**
			 * Adds a new uniform if no uniform with the name exists and
			 * returns it, or returns the existing uniform with this name.
			 * @param name Name of the new uniform.
			 * @return Uniform with the name.
			 */
			Uniform &add(const std::string &name);

			/**
			 * Iterates through all uniforms in this instance and looks for them
			 * in the other instance. If the uniform exists in "data", then the
			 * uniform in this instance is set to its value.
			 */
			void setValues(const UniformData &data);

			UniformData &operator=(const UniformData &data);
			/**
			 * Returns the uniform with the given name, or a dummy uniform if
			 * no uniform was found.
			 * @param name Name of the uniform.
			 * @return Uniform with the given name or dummy uniform.
			 */
			Uniform &operator[](const std::string &name);
			/**
			 * Returns the uniform with the given name, or a dummy uniform if
			 * no uniform was found.
			 * @param name Name of the uniform.
			 * @return Uniform with the given name or dummy uniform.
			 */
			const Uniform &operator[](const std::string &name) const;

			typedef core::HashMap<std::string, Uniform>::Type UniformMap;
			/**
			 * Returns all uniforms in a map.
			 * @return Uniform map.
			 */
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
