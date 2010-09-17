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
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	ShaderText::ShaderText(res::ResourceManager *rmgr,
	                       const std::string &name)
		: Resource(rmgr, name), flagdefaults(0)
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
			core::FileSystem::Ptr fs = getManager()->getFileSystem();
			std::string path = getPath();
			std::string directory = core::FileSystem::getDirectory(path);
			// Resolve includes
			std::string processed;
			if (!resolveIncludes(text, processed, directory))
				return false;
			texts[name] = processed;
		}
		else
		{
			texts[name] = text;
		}
		return true;
	}

	bool ShaderText::addContext(const std::string &name,
	                            const std::string &vs,
	                            const std::string &fs,
	                            const std::string &gs,
	                            const std::string &ts,
	                            const BlendMode::List &blendmode)
	{	
		Context context = {
			vs, fs, gs, ts, blendmode
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
			getManager()->getLog()->warning("%s: Too many flags, flag omitted.",
			                            getName().c_str());
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
		uniforms.add(name).set(type, defaultvalue);
	}
	void ShaderText::addTexture(const std::string &name)
	{
		textures.push_back(name);
	}

	unsigned int ShaderText::getFlags(const std::string &flagsset)
	{
		std::istringstream stream(flagsset);
		unsigned int output = flagdefaults;
		// Tokenize flag string
		while(!stream.eof())
		{
			std::string flag;
			stream >> flag;
			if (flag == "")
				break;
			size_t equalsign = flag.find("=");
			if (equalsign == std::string::npos)
			{
				getManager()->getLog()->warning("Invalid flag string \"%s\".",
				                            flag.c_str());
				continue;
			}
			// Get flag name
			std::string flagname = flag.substr(0, equalsign);
			int flagindex = -1;
			for (unsigned int i = 0; i < flags.size(); i++)
			{
				if (flags[i] == flagname)
				{
					flagindex = i;
					break;
				}
			}
			if (flagindex == -1)
			{
				getManager()->getLog()->warning("Unknown flag: \"%s\".",
				                            flagname.c_str());
				continue;
			}
			// Get value
			bool flagvalue = false;
			if (flag.substr(equalsign + 1) == "true")
				flagvalue = true;
			// Set value in the bitset
			if (flagvalue)
				output |= 1 << flagindex;
			else
				output &= ~(1 << flagindex);
		}
		return output;
	}

	void ShaderText::updateShaders()
	{
		// TODO: Do we need this?
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
		// Create shader
		// TODO: Do not reupload the shader if it already exists
		Shader::Ptr shader = getManager()->getOrCreate<Shader>("Shader",
		                                                       shadername.str());
		// Set flags
		std::string flagtext;
		for (unsigned int i = 0; i < this->flags.size(); i++)
		{
			flagtext += "#define " + this->flags[i] + " ";
			if ((flags && (1 << i)) != 0)
				flagtext += "1\n";
			else
				flagtext += "0\n";
		}
		// Check whether texts exists
		if (texts.find(ctx.vs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx.vs.c_str());
			return false;
		}
		if (texts.find(ctx.fs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx.fs.c_str());
			return false;
		}
		if (ctx.gs != "" && texts.find(ctx.gs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx.gs.c_str());
			return false;
		}
		if (ctx.ts != "" && texts.find(ctx.ts) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx.ts.c_str());
			return false;
		}
		// Set shader data
		shader->setBlendMode(ctx.blendMode);
		shader->setVertexShader(flagtext + texts[ctx.vs]);
		shader->setFragmentShader(flagtext + texts[ctx.fs]);
		if (ctx.gs != "")
			shader->setGeometryShader(flagtext + texts[ctx.gs]);
		if (ctx.ts != "")
			shader->setTesselationShader(flagtext + texts[ctx.ts]);
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
		// Open XML file
		TiXmlDocument xml(path.c_str());
		if (!loadResourceFile(xml))
		{
			finishLoading(false);
			return false;
		}
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
			// TODO: Load texts directly from files
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
			
			TiXmlElement *elmChild = element->FirstChildElement("Blend");
			
			std::string blendmodestr = "";
			BlendMode::List blendMode = BlendMode::Solid;
			
			if ( elmChild )
			{
				blendmodestr = elmChild->Attribute("mode");
				
				if (blendmodestr.compare("Additive") == 0)
					blendMode = BlendMode::Additive;
			}
			
			// Add context
			addContext(name, vsname, fsname, gsname, tsname, blendMode);
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
			// Check whether we are supposed to create an array
			// TODO: This is ugly and slow
			const char *countstr = element->Attribute("count");
			if (countstr)
			{
				// Add uniform array
				// In this case we do not support default data
				unsigned int count = atoi(countstr);
				for (unsigned int i = 0; i < count; i++)
				{
					std::ostringstream namestream;
					namestream << name << "[" << i << "]";
					addUniform(namestream.str(), type);
				}
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
		for (TiXmlNode *node = root->FirstChild("Flag");
		     node != 0;
		     node = root->IterateChildren("Flag", node))
		{
			TiXmlElement *element = node->ToElement();
			if (!element)
				continue;
			// Read text
			const char *name = element->Attribute("name");
			if (!name)
			{
				getManager()->getLog()->warning("%s: Flag name missing.",
				                                getName().c_str());
				continue;
			}
			// Read default value
			bool defvalue = false;
			const char *defstr = element->Attribute("default");
			if (defstr && !strcmp(defstr, "true"))
				defvalue = true;
			// Add flag
			addFlag(name, defvalue);
		}
		finishLoading(true);
		return true;
	}

	bool ShaderText::resolveIncludes(const std::string &text,
	                                 std::string &output,
	                                 const std::string &directory)
	{
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		// Reset output
		output = "";
		// Go through the string line by line
		// TODO: Is there a faster way to do this?
		std::istringstream stream(text);
		std::string line;
		while (!stream.eof())
		{
			std::getline(stream, line);
			// Find #include
			size_t includebegin = line.find("#include");
			if (includebegin != std::string::npos)
			{
				if (line.find_first_not_of(" \t") < includebegin)
				{
					// #include not at the beginning of the line
					output += line + "\n";
					continue;
				}
				// Find area of file name
				size_t filebegin = line.find("\"", includebegin + 8);
				if (filebegin == std::string::npos
				 || line.find_first_not_of(" \t", includebegin + 8) < filebegin)
				{
					// Non-whitespace chars between #include and the file
					getManager()->getLog()->error("%s: Invalid #include.",
					                          getName().c_str());
					return false;
				}
				size_t fileend = line.find("\"", filebegin + 1);
				if (fileend == std::string::npos)
				{
					// Missing end of file name
					getManager()->getLog()->error("%s: Unterminated #include.",
					                          getName().c_str());
					return false;
				}
				// Data after the file name (only comments allowed)
				size_t otherdatabegin = line.find_first_not_of(" \t", fileend + 1);
				if (otherdatabegin != std::string::npos
				 && line.find("//", fileend + 1) != otherdatabegin
				 && line.find("/*", fileend + 1) != otherdatabegin)
				{
					// Non-whitespace chars between #include and the file
					getManager()->getLog()->error("%s: Garbage after #include.",
					                          getName().c_str());
					return false;
				}
				std::string otherdata = "";
				if (otherdatabegin != std::string::npos)
					line.substr(otherdatabegin);
				// Get file name
				std::string filename = line.substr(filebegin + 1,
				                                   fileend - filebegin - 1);
				filename = fs->getPath(filename, directory);
				// Open file
				core::File::Ptr file = fs->open(filename,
				                                core::FileAccess::Read | core::FileAccess::Text);
				if (!file)
				{
					getManager()->getLog()->error("#include: Could not open file \"%s\".",
					                               filename.c_str());
					return false;
				}
				std::string loadedtext = file->readAll();
				// Recursively parse the file text
				std::string newdir = core::FileSystem::getDirectory(filename);
				std::string processed;
				if (!resolveIncludes(loadedtext, processed, newdir))
					return false;
				// Add text to output
				output += processed + "\n";
				if (!otherdata.empty())
					output += otherdata + "\n";
			}
			else
				output += line + "\n";
		}
		return true;
	}
}
}
