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

#include "CoreRender/render/UniformData.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	Uniform::Uniform(const std::string &name)
		: name(name), type(ShaderVariableType::Invalid), data(0),
		shaderhandle(-1)
	{
	}
	Uniform::Uniform(const std::string &name,
	                 ShaderVariableType::List type,
	                 float *data)
		: name(name), type(type), shaderhandle(-1)
	{
		unsigned int size = ShaderVariableType::getSize(type);
		this->data = new float[size];
		memcpy(this->data, data, size * sizeof(float));
	}
	Uniform::Uniform(const Uniform &other)
		: name(other.name), type(other.type), shaderhandle(other.shaderhandle)
	{
		unsigned int size = ShaderVariableType::getSize(other.type);
		this->data = new float[size];
		memcpy(this->data, other.data, size * sizeof(float));
	}
	Uniform::~Uniform()
	{
		if (data)
			delete[] data;
	}

	Uniform &Uniform::operator=(float f)
	{
		if (type != ShaderVariableType::Float)
		{
			type = ShaderVariableType::Float;
			if (data)
				delete[] data;
			data = new float[1];
		}
		data[0] = f;
		return *this;
	}
	Uniform &Uniform::operator=(int i)
	{
		// TODO: int uniforms
		if (type != ShaderVariableType::Float)
		{
			type = ShaderVariableType::Float;
			if (data)
				delete[] data;
			data = new float[1];
		}
		data[0] = (float)i;
		return *this;
	}
	Uniform &Uniform::operator=(const math::Vector2F &v)
	{
		if (type != ShaderVariableType::Float2)
		{
			type = ShaderVariableType::Float2;
			if (data)
				delete[] data;
			data = new float[2];
		}
		data[0] = v.x;
		data[1] = v.y;
		return *this;
	}
	Uniform &Uniform::operator=(const math::Vector3F &v)
	{
		if (type != ShaderVariableType::Float3)
		{
			type = ShaderVariableType::Float3;
			if (data)
				delete[] data;
			data = new float[3];
		}
		data[0] = v.x;
		data[1] = v.y;
		data[2] = v.z;
		return *this;
	}
	Uniform &Uniform::operator=(const math::Vector2I &v)
	{
		// TODO: int uniforms
		if (type != ShaderVariableType::Float2)
		{
			type = ShaderVariableType::Float2;
			if (data)
				delete[] data;
			data = new float[2];
		}
		data[0] = (float)v.x;
		data[1] = (float)v.y;
		return *this;
	}
	Uniform &Uniform::operator=(const math::Vector3I &v)
	{
		// TODO: int uniforms
		if (type != ShaderVariableType::Float3)
		{
			type = ShaderVariableType::Float3;
			if (data)
				delete[] data;
			data = new float[3];
		}
		data[0] = (float)v.x;
		data[1] = (float)v.y;
		data[2] = (float)v.z;
		return *this;
	}
	Uniform &Uniform::operator=(const Uniform &other)
	{
		// TODO: Disallow type changes?
		unsigned int size = ShaderVariableType::getSize(other.type);
		if (other.type != type)
		{
			if (data)
				delete[] data;
			type = other.type;
			data = new float[size];
		}
		memcpy(data, other.data, size * sizeof(float));
		// TODO: How do we want to handle copies?
		//shaderhandle = other.shaderhandle;
		return *this;
	}
	void Uniform::set(ShaderVariableType::List type,
	                  float *data)
	{
		unsigned int size = ShaderVariableType::getSize(type);
		if (this->type != type)
		{
			if (this->data)
				delete[] this->data;
			this->type = type;
			this->data = new float[size];
		}
		if (data)
			memcpy(this->data, data, size * sizeof(float));
		else
			memset(this->data, 0, size * sizeof(float));
	}

	UniformData::UniformData()
		: invalid("_invalid")
	{
	}
	UniformData::UniformData(const UniformData &data)
		: invalid("_invalid")
	{
		uniforms = data.uniforms;
	}
	UniformData::~UniformData()
	{
	}

	Uniform &UniformData::add(const std::string &name)
	{
		std::pair<UniformMap::iterator, bool> inserted;
		inserted = uniforms.insert(std::make_pair(name, Uniform(name)));
		if (inserted.second)
			return inserted.first->second;
		UniformMap::iterator it = uniforms.find(name);
		it->second = Uniform(name);
		return it->second;
	}

	void UniformData::setValues(const UniformData &other)
	{
		for (UniformMap::iterator it = uniforms.begin();
		     it != uniforms.end(); it++)
		{
			UniformMap::const_iterator it2 = other.uniforms.find(it->first);
			if (it2 == other.uniforms.end())
				continue;
			it->second = it2->second;
		}
	}

	UniformData &UniformData::operator=(const UniformData &data)
	{
		uniforms = data.uniforms;
		return *this;
	}
	Uniform &UniformData::operator[](const std::string &name)
	{
		UniformMap::iterator it = uniforms.find(name);
		if (it == uniforms.end())
			return invalid;
		return it->second;
	}
	const Uniform &UniformData::operator[](const std::string &name) const
	{
		UniformMap::const_iterator it = uniforms.find(name);
		if (it == uniforms.end())
			return invalid;
		return it->second;
	}
}
}
