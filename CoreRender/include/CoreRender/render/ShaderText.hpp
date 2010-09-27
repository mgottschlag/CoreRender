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
#include "BlendMode.hpp"
#include "Shader.hpp"
#include "ShaderVariableType.hpp"
#include "UniformData.hpp"

#include <map>

namespace cr
{
namespace render
{
	class Renderer;

	/**
	 * Class containing shader texts and shader variable information. This class
	 * then can produce Shader instances for certain contexts/flag combinations.
	 *
	 * @todo Explanation on how to define shaders.
	 *
	 * This class should be created via ResourceManager::getOrCreate().
	 */
	class ShaderText : public res::Resource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			ShaderText(res::ResourceManager *rmgr,
			           const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~ShaderText();

			/**
			 * Adds a raw shader text that can later be used by contexts. This
			 * function also can recursively check for "#include" and include
			 * the referenced shader files. Not that including other files
			 * currently produces unusable line numbers in shader compiler
			 * reports in the log.
			 * @param name Name of the shader text.
			 * @param text Content of the shader text.
			 * @param autoinclude If true, the function acts as a preprocessor
			 * and resolves all #include lines.
			 * @return Returns false if the function fails, usually because an
			 * include statement could not be resolved.
			 */
			bool addText(const std::string &name,
			             const std::string &text,
			             bool autoinclude = false);

			/**
			 * Adds a context to the context list. The context contains several
			 * shader texts which are then used to create shaders for this
			 * context.
			 * @param name Name of the context to be created.
			 * @param vs Vertex shader text name.
			 * @param fs Fragment (pixel) shader text name.
			 * @param gs Geometry shader text name. Leave as "" for no geometry
			 * shader.
			 * @param ts Tesselation shader text name. Leave as "" for no
			 * tesselation shader.
			 * @return Always returns true as this function currently does not
			 * check whether the texts actually exist.
			 * @todo Currently shader creation happens in getShader(). It has
			 * to be done here though as getShader() is called too late.
			 */
			bool addContext(const std::string &name,
			                const std::string &vs,
			                const std::string &fs,
			                const std::string &gs = "",
			                const std::string &ts = "",
			                const BlendMode::List &blendmode = BlendMode::Solid);
			/**
			 * Returns whether this material has a certain context.
			 * @param name Name of the context.
			 * @return True if the context exists for this material.
			 */
			bool hasContext(const std::string &name);

			/**
			 * Adds a flag to the material. A flag can then be checked in
			 * shaders via "#if FlagName".
			 * @param flag Name of the flag.
			 * @param defaultvalue Default value of the flag.
			 */
			void addFlag(const std::string &flag, bool defaultvalue);

			/**
			 * Adds an attrib to the shaders. Only attribs which are added like
			 * this can be used in layouts/shaders.
			 * @param name Name of the attrib.
			 */
			void addAttrib(const std::string &name);
			/**
			 * Adds a new uniform to the shaders.
			 * @param name Name of the uniform.
			 * @param type Type of the uniform variable.
			 * @param defaultvalue If not 0, this data is used as the default
			 * value of the uniform.
			 */
			void addUniform(const std::string &name,
			                ShaderVariableType::List type,
			                float *defaultvalue = 0);
			/**
			 * Adds a texture sampler entry to all shaders. Only textures which
			 * are added like this get shader handles and can be used.
			 */
			void addTexture(const std::string &name);

			/**
			 * Returns the flag bitset for a string containing flag changes.
			 * The string has to be space separated, with elements in the form
			 * "Flagname=true|false", e.g. "Skinning=true NormalMapping=false".
			 * If a flag is not set in the flag string, its default value is
			 * used.
			 * @note Do not add additional spaces!
			 * @param flagsset Flag change string.
			 * @return Bitset containing all flag values.
			 */
			unsigned int getFlags(const std::string &flagsset = "");

			/**
			 * Updates all existing shaders.
			 */
			void updateShaders();

			/**
			 * Creates all available shaders for a specific combination of
			 * flags. This function is necessary because the shader cannot be
			 * created directly in getShader() as this function usually is
			 * called while already rendering, at a time where no new resources
			 * can be added anymore.
			 * @param flags The flag values for which shaders are generated.
			 */
			void prepareShaders(unsigned int flags);

			/**
			 * Returns a shader instance for a context, taking a certain flag
			 * set into account. Not that this must not called after
			 * GraphicsEngine::beginFrame() as it might create a new resource.
			 * @param context Shader context.
			 * @param flags Flag bitset as returned by getFlags().
			 * @return Shader or 0 if no shader could be created.
			 * @todo This (and other functions here) needs to be made threadsafe.
			 */
			Shader::Ptr getShader(const std::string &context,
			                      unsigned int flags);

			virtual const char *getType()
			{
				return "ShaderText";
			}

			/**
			 * Returns the default uniform data for the shaders created by this
			 * shader text.
			 * @return Default uniform data.
			 */
			const UniformData &getUniformData()
			{
				return uniforms;
			}

			virtual bool load();

			typedef core::SharedPointer<ShaderText> Ptr;
		private:
			bool resolveIncludes(const std::string &text,
			                     std::string &output,
			                     const std::string &directory);

			struct Context
			{
				std::string vs;
				std::string fs;
				std::string gs;
				std::string ts;
				BlendMode::List blendMode;
			};

			std::map<std::string, std::string> texts;
			std::map<std::string, Context> contexts;

			std::vector<std::string> flags;
			unsigned int flagdefaults;
			std::vector<std::string> attribs;
			std::vector<std::string> textures;

			std::vector<Shader::Ptr> shaders;

			UniformData uniforms;
	};
}
}

#endif
