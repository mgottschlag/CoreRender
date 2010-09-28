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

#include "CoreRender/render/Material.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/Texture2D.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	Material::Material(res::ResourceManager *rmgr,
	                   const std::string &name)
		: Resource(rmgr, name)
	{
	}
	Material::~Material()
	{
	}

	void Material::setShader(ShaderText::Ptr shader)
	{
		this->shader = shader;
		shader->prepareShaders(getShaderFlags());
	}
	ShaderText::Ptr Material::getShader()
	{
		return shader;
	}

	void Material::addTexture(const std::string name, Texture::Ptr texture)
	{
		TextureInfo info;
		info.name = name;
		info.texture = texture;
		textures.push_back(info);
	}
	Texture::Ptr Material::getTexture(const std::string &name)
	{
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			if (textures[i].name == name)
			{
				return textures[i].texture;
			}
		}
		return 0;
	}
	const std::vector<Material::TextureInfo> &Material::getTextures()
	{
		return textures;
	}

	bool Material::load()
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
		TiXmlNode *root = xml.FirstChild("Material");
		if (!root)
		{
			getManager()->getLog()->error("%s: <Material> not found.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		{
			// Get shader
			TiXmlElement *shaderelem;
			for (TiXmlNode *node = root->FirstChild("Shader");
			     node != 0;
			     node = root->IterateChildren("Shader", node))
			{
				getManager()->getLog()->debug("%s: Text.",
				                              getName().c_str());
				shaderelem = node->ToElement();
				if (shaderelem)
					break;
			}
			if (!shaderelem)
			{
				getManager()->getLog()->error("%s: No shader given.",
				                              getName().c_str());
				finishLoading(false);
				return false;
			}
			// Get shader info
			const char *shaderfile = shaderelem->Attribute("file");
			if (!shaderfile)
			{
				getManager()->getLog()->error("%s: Shader file missing.",
				                              getName().c_str());
				finishLoading(false);
				return false;
			}
			const char *flagattrib = shaderelem->Attribute("flags");
			std::string flags;
			if (flagattrib)
				flags = flagattrib;
			// Load shader
			ShaderText::Ptr shader;
			shader = getManager()->getOrLoad<ShaderText>("ShaderText", shaderfile);
			setShader(shader);
			setShaderFlags(flags);
		}
		// Load textures
		for (TiXmlNode *node = root->FirstChild("Texture");
		     node != 0;
		     node = root->IterateChildren("Texture", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read texture info
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Texture name missing.",
				                                getName().c_str());
				continue;
			}
			const char *file = element->Attribute("file");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Texture file missing.",
				                                getName().c_str());
				continue;
			}
			// Load texture
			// TODO: Texture types
			Texture::Ptr texture;
			core::FileSystem::Ptr fs = getManager()->getFileSystem();
			texture = getManager()->getOrLoad<Texture2D>("Texture2D",
			                                             fs->getPath(file, directory));
			// Add texture
			addTexture(name, texture);
		}
		// Load uniforms
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
					char separator;
					stream >> separator;
				}
			}
			// Add uniform
			uniforms.add(name).set(type, defdata);
			delete[] defdata;
		}
		finishLoading(true);
		return true;
	}

	bool Material::waitForLoading(bool recursive, bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		// Wait for the shader
		bool result = true;
		if (shader)
			result = result && shader->waitForLoading(recursive, highpriority);
		// Wait for the textures
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			result = result && textures[i].texture->waitForLoading(recursive,
			                                                       highpriority);
		}
		return result;
	}
}
}
