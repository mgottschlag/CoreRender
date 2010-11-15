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
#include "CoreRender/res/ResourceManager.hpp"
#include "../3rdparty/tinyxml.h"

#include <sstream>

namespace cr
{
namespace render
{
	Shader::Shader(UploadManager &uploadmgr,
	               res::ResourceManager *rmgr,
	               const std::string &name)
		: RenderResource(uploadmgr, rmgr, name), flagdefaults(0),
		supportedflags(0), supportsskinning(false), supportsinstancing(false)
	{
	}
	Shader::~Shader()
	{
	}

	bool Shader::addText(const std::string &name,
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
		// Register the resource for reuploading
		registerUpload();
		return true;
	}

	bool Shader::addContext(const std::string &name,
	                        const std::string &vs,
	                        const std::string &fs,
	                        const std::string &gs,
	                        const std::string &ts,
	                        BlendMode::List blendmode,
	                        bool depthwrite,
	                        DepthTest::List depthtest)
	{
		unsigned int namehandle = getManager()->getNameRegistry().getContext(name);
		Context context = {
			namehandle, vs, fs, gs, ts, blendmode, depthwrite, depthtest,
			std::vector<ShaderCombination::Ptr>()
		};
		// Store context info
		contexts.push_back(context);
		// Register the resource for reuploading
		registerUpload();
		return true;
	}

	unsigned int Shader::getFlagIndex(const std::string &name)
	{
		if (name == "Skinning")
			return 32;
		if (name == "Instancing")
			return 32;
		tbb::mutex::scoped_lock lock(flagmutex);
		// Look for an existing flag
		for (unsigned int i = 0; i < compilerflags.size(); i++)
		{
			if (compilerflags[i] == name)
				return i;
		}
		unsigned int flagcount = compilerflags.size();
		// We limit the number of flags to 32 so that they fit into unsigned int
		if (flagcount == 32)
			return 32;
		compilerflags.push_back(name);
		return flagcount;
	}
	void Shader::setFlagValue(unsigned int index, bool enabled)
	{
		if (index >= 32)
			return;
		if (enabled)
			flagdefaults |= 1 << index;
		else
			flagdefaults &= ~(1 << index);
	}

	void Shader::addAttrib(const std::string &name)
	{
		unsigned int namehandle = getManager()->getNameRegistry().getAttrib(name);
		currentinfo.attribs.push_back(namehandle);
		// Update shader combinations
		// TODO
		reupload();
	}
	void Shader::addUniform(const std::string &name,
	                        ShaderVariableType::List type,
	                        float *defaultvalue)
	{
		Uniform uniform;
		uniform.name = name;
		uniform.size = ShaderVariableType::getSize(type);
		memset(uniform.defvalue, 0, 16 * sizeof(float));
		if (defaultvalue)
			memcpy(uniform.defvalue, defaultvalue, uniform.size * sizeof(float));
		currentinfo.uniforms.push_back(uniform);
		// Update shader combinations
		// TODO
		reupload();
	}
	void Shader::addSampler(const std::string &name)
	{
		Sampler sampler;
		sampler.name = name;
		sampler.texunit = 0;
		sampler.flags = 0;
		// TODO: Correct type
		sampler.type = TextureType::Texture2D;
		currentinfo.samplers.push_back(sampler);
		// Update shader combinations
		// TODO
		reupload();
	}

	void Shader::getFlags(const std::string &flagstr,
	                      unsigned int &flagmask,
	                      unsigned int &flagvalue)
	{
		std::istringstream stream(flagstr);
		unsigned int output = 0;
		unsigned int outputmask = 0;
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
			unsigned int flagindex = getFlagIndex(flagname);
			if (flagindex == 32)
				continue;
			// Get value
			bool flagvalue = false;
			if (flag.substr(equalsign + 1) == "true")
				flagvalue = true;
			// Set value in the bitset
			outputmask |= 1 << flagindex;
			if (flagvalue)
				output |= 1 << flagindex;
		}
		flagmask = outputmask;
		flagvalue = output;
	}

	ShaderCombination::Ptr Shader::getCombination(unsigned int context,
	                                              unsigned int flagmask,
	                                              unsigned int flagvalue,
	                                              bool instancing,
	                                              bool skinning)
	{
		// TODO: Also check capabilities
		if (!supportsInstancing())
			instancing = false;
		if (!supportsSkinning())
			skinning = false;
		// Where no flags were set, we use the default flags
		unsigned int flags = (flagdefaults & ~flagmask) | (flagvalue & flagmask);
		flags &= supportedflags;
		// Get context
		Context *ctx = 0;
		for (unsigned int i = 0; i < contexts.size(); i++)
		{
			if (contexts[i].name == context)
			{
				ctx = &contexts[i];
				break;
			}
		}
		if (!ctx)
		{
			std::string ctxstr = getManager()->getNameRegistry().getContext(context);
			getManager()->getLog()->error("%s: Context %s not found.",
			                              getName().c_str(),
			                              ctxstr.c_str());
			return false;
		}
		tbb::mutex::scoped_lock lock(combinationmutex);
		// Look whether the combination already exists
		for (unsigned int i = 0; i < ctx->combinations.size(); i++)
		{
			if (ctx->combinations[i]->compilerflags == flags
				&& ctx->combinations[i]->instancing == instancing
				&& ctx->combinations[i]->skinning == skinning)
				return ctx->combinations[i];
		}
		// Create shader
		ShaderCombination::Ptr combination = new ShaderCombination(getUploadManager());
		combination->compilerflags = flags;
		combination->instancing = instancing;
		combination->skinning = skinning;
		// Set flags
		std::string flagtext;
		for (unsigned int i = 0; i < compilerflags.size(); i++)
		{
			flagtext += "#define " + compilerflags[i] + " ";
			if ((flags && (1 << i)) != 0)
				flagtext += "1\n";
			else
				flagtext += "0\n";
		}
		if (skinning)
			flagtext += "#define Skinning 1\n";
		else
			flagtext += "#define Skinning 0\n";
		if (instancing)
			flagtext += "#define Instancing 1\n";
		else
			flagtext += "#define Instancing 0\n";
		// Check whether texts exists
		if (texts.find(ctx->vs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx->vs.c_str());
			return 0;
		}
		if (texts.find(ctx->fs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx->fs.c_str());
			return 0;
		}
		if (ctx->gs != "" && texts.find(ctx->gs) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx->gs.c_str());
			return 0;
		}
		if (ctx->ts != "" && texts.find(ctx->ts) == texts.end())
		{
			getManager()->getLog()->error("Text \"%s\" not found.", ctx->ts.c_str());
			return 0;
		}
		// Set shader data
		combination->currentdata.blendmode = ctx->blendmode;
		combination->currentdata.depthwrite = ctx->depthwrite;
		combination->currentdata.depthtest = ctx->depthtest;
		combination->currentdata.vs = flagtext + texts[ctx->vs];
		combination->currentdata.fs = flagtext + texts[ctx->fs];
		if (ctx->gs != "")
			combination->currentdata.gs = flagtext + texts[ctx->gs];
		if (ctx->ts != "")
			combination->currentdata.ts = flagtext + texts[ctx->ts];
		// Finish shader
		combination->shader = this;
		combination->registerUpload();
		ctx->combinations.push_back(combination);
		return combination;
	}

	bool Shader::load()
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
			// Read blend mode
			TiXmlElement *child = element->FirstChildElement("Blend");
			BlendMode::List blendmode = BlendMode::Replace;
			if (child)
			{
				const char *blendmodestr = child->Attribute("mode");
				if (blendmodestr)
				{
					if (!strcmp(blendmodestr, "Add"))
						blendmode = BlendMode::Add;
					else if (!strcmp(blendmodestr, "AddBlended"))
						blendmode = BlendMode::AddBlended;
					else if (!strcmp(blendmodestr, "Blend"))
						blendmode = BlendMode::Blend;
					else if (!strcmp(blendmodestr, "Replace"))
						blendmode = BlendMode::Replace;
					else if (!strcmp(blendmodestr, "Multiply"))
						blendmode = BlendMode::Multiply;
					else if (!strcmp(blendmodestr, "Minimum"))
						blendmode = BlendMode::Minimum;
					else if (!strcmp(blendmodestr, "Maximum"))
						blendmode = BlendMode::Maximum;
				}
			}
			// Read depth mode info
			bool depthwrite = true;
			DepthTest::List depthtest = DepthTest::Less;
			child = element->FirstChildElement("Depth");
			if (child)
			{
				const char *writestr = child->Attribute("write");
				if (writestr && strcmp(writestr, "true"))
					depthwrite = false;
				const char *teststr = child->Attribute("test");
				if (writestr)
				{
					if (!strcmp(teststr, "Always"))
						depthtest = DepthTest::Always;
					else if (!strcmp(teststr, "Equal"))
						depthtest = DepthTest::Equal;
					else if (!strcmp(teststr, "Less"))
						depthtest = DepthTest::Less;
					else if (!strcmp(teststr, "LessEqual"))
						depthtest = DepthTest::LessEqual;
					else if (!strcmp(teststr, "Greater"))
						depthtest = DepthTest::Greater;
					else if (!strcmp(teststr, "GreaterEqual"))
						depthtest = DepthTest::GreaterEqual;
				}
			}
			// Add context
			addContext(name,
			           vsname,
			           fsname,
			           gsname,
			           tsname,
			           blendmode,
			           depthwrite,
			           depthtest);
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
			addSampler(name);
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
			// Skinning and Instancing are treated separately
			if (!strcmp(name, "Skinning"))
			{
				supportsskinning = true;
				continue;
			}
			if (!strcmp(name, "Instancing"))
			{
				supportsinstancing = true;
				continue;
			}
			// Add flag
			unsigned int flagindex = getFlagIndex(name);
			if (flagindex == 32)
			{
				getManager()->getLog()->warning("%s: Too many flags, flag %s omitted.",
				                                getName().c_str(), name);
				continue;
			}
			setFlagValue(flagindex, defvalue);
			supportedflags |= 1 << flagindex;
		}
		// Finish loading
		finishLoading(true);
		return true;
	}
	void Shader::upload(void *data)
	{
		ShaderInfo *uploaddata = (ShaderInfo*)data;
		uploadedinfo = *uploaddata;
		delete uploaddata;
	}

	void *Shader::getUploadData()
	{
		return new ShaderInfo(currentinfo);
	}

	void Shader::onDelete()
	{
		// We have to drop all references to ShaderCombination instances here
		// because these need to be deleted at the same time as this shader
		for (unsigned int i = 0; i < contexts.size(); i++)
		{
			contexts[i].combinations.clear();
		}
		RenderResource::onDelete();
	}

	bool Shader::resolveIncludes(const std::string &text,
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

	void Shader::reupload()
	{
		// Register everything for upload
		registerUpload();
		for (unsigned int i = 0; i < contexts.size(); i++)
		{
			for (unsigned int j = 0; j < contexts[i].combinations.size(); j++)
			{
				contexts[i].combinations[j]->registerUpload();
			}
		}
	}
}
}
