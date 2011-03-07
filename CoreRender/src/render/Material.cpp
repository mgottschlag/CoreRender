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
#include "CoreRender/render/Texture.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	Material::Material(UploadManager &uploadmgr,
	                   res::ResourceManager *rmgr,
	                   const std::string &name)
		: RenderResource(uploadmgr, rmgr, name), shaderflagmask(0),
		shaderflagvalue(0), uploadeddata(0)
	{
	}
	Material::~Material()
	{
		if (uploadeddata)
		{
			delete[] uploadeddata->textures;
			delete uploadeddata;
		}
	}

	void Material::setShader(Shader::Ptr shader)
	{
		this->shader = shader;
		shader->getFlags(shaderflags, shaderflagmask, shaderflagvalue);
	}
	Shader::Ptr Material::getShader()
	{
		return shader;
	}

	void Material::addTexture(const std::string name, Texture::Ptr texture)
	{
		TextureInfo info;
		info.name = name;
		info.texture = texture;
		textures.push_back(info);
		registerUpload();
	}
	void Material::setTexture(const std::string name, Texture::Ptr texture)
	{
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			if (textures[i].name == name)
			{
				textures[i].texture = texture;
			}
		}
		registerUpload();
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

	void Material::setUniform(std::string name, unsigned int size, float *data)
	{
		if (size > 16)
		{
			getManager()->getLog()->error("%s: setUniform(): Wrong size.",
			                              getName().c_str());
			return;
		}
		// Search for an existing entry in the uniform list
		// This currently does not use a hash map or sth like that because of
		// the low number of uniforms which are usually used
		for (unsigned int i = 0; i < uniforms.size(); i++)
		{
			if (uniforms[i].name == name)
			{
				uniforms[i].size = size;
				memcpy(uniforms[i].data, data, size * sizeof(float));
				return;
			}
		}
		// If the uniform was not found, create a new list entry
		UniformInfo uniform;
		uniform.name = name;
		uniform.size = size;
		memcpy(uniform.data, data, size * sizeof(float));
		uniforms.push_back(uniform);
	}
	void Material::removeUniform(std::string name)
	{
		for (unsigned int i = 0; i < uniforms.size(); i++)
		{
			if (uniforms[i].name == name)
			{
				uniforms.erase(uniforms.begin() + i);
				registerUpload();
				return;
			}
		}
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
			TiXmlElement *shaderelem = root->FirstChildElement("Shader");
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
			Shader::Ptr shader;
			shader = getManager()->getOrLoad<Shader>("Shader", shaderfile);
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
			texture = getManager()->getOrLoad<Texture>("Texture",
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
			setUniform(name, size, defdata);
			delete[] defdata;
		}
		finishLoading(true);
		return true;
	}

	bool Material::waitForLoading(bool recursive, bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		if (!recursive)
			return true;
		// Wait for the shader
		bool result = true;
		if (shader && !shader->waitForLoading(recursive, highpriority))
			result = false;
		// Wait for the textures
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			if (!textures[i].texture->waitForLoading(recursive, highpriority))
				result = false;
		}
		return result;
	}

	void Material::upload(void *data)
	{
		if (uploadeddata)
		{
			delete[] uploadeddata->textures;
			delete uploadeddata;
		}
		uploadeddata = (TextureList*)data;
	}
	void *Material::getUploadData()
	{
		TextureList *list = new TextureList;
		list->texturecount = textures.size();
		list->textures = new TextureInfo[list->texturecount];
		for (unsigned int i = 0; i < list->texturecount; i++)
			list->textures[i] = textures[i];
		return list;
	}

	res::Resource::Ptr Material::clone(std::string name)
	{
		if (name == "")
			name = getManager()->getInternalName();
		Material::Ptr material = new Material(getUploadManager(),
		                                      getManager(),
											  name);
		material->shader = shader;
		material->shaderflags = shaderflags;
		material->shaderflagmask = shaderflagmask;
		material->shaderflagvalue = shaderflagvalue;

		material->textures = textures;
		material->uniforms = uniforms;
		material->registerUpload();
		return material;
	}

	res::Resource::Ptr Material::cloneDeep(std::string name)
	{
		// Clone used resources
		res::Resource::Ptr shadercopy = shader->cloneDeep();
		if (!shadercopy)
			return 0;
		std::vector<res::Resource::Ptr> texturecopy(textures.size());
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			if (textures[i].texture)
			{
				texturecopy[i] = textures[i].texture->cloneDeep();
				if (!texturecopy[i])
					return 0;
			}
		}
		// Clone the resource
		if (name == "")
			name = getManager()->getInternalName();
		Material::Ptr material = new Material(getUploadManager(),
											  getManager(),
											  name);
		material->shader = (Shader*)shadercopy.get();
		material->shaderflags = shaderflags;
		material->shaderflagmask = shaderflagmask;
		material->shaderflagvalue = shaderflagvalue;

		material->textures = textures;
		for (unsigned int i = 0; i < textures.size(); i++)
		{
			material->textures[i].texture = (Texture*)texturecopy[i].get();
		}
		material->uniforms = uniforms;
		material->registerUpload();
		return material;
	}
}
}
