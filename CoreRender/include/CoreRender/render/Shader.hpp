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
#include "BlendMode.hpp"
#include "ShaderVariableType.hpp"
#include "UniformData.hpp"
#include "DepthTest.hpp"
#include "Texture.hpp"
#include "ShaderCombination.hpp"

#include <map>

namespace cr
{
namespace render
{
	/**
	 * Class containing shader texts and shader variable information. This class
	 * then can produce ShaderCombination instances for certain contexts/flag
	 * combinations.
	 *
	 * @todo Explanation on how to define shaders.
	 *
	 * This class should be created via ResourceManager::getOrCreate().
	 */
	class Shader : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Shader(UploadManager &uploadmgr,
			       res::ResourceManager *rmgr,
			       const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Shader();

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
			 * @param blendmode Blend mode which this context should use.
			 * @param depthwrite If true, the material writes into the depth
			 * buffer.
			 * @param depthtest The depth test which should be used for this
			 * material. This can be used to always render independent of the
			 * contents of the depth buffer.
			 * @return Always returns true as this function currently does not
			 * check whether the texts actually exist.
			 */
			bool addContext(const std::string &name,
			                const std::string &vs,
			                const std::string &fs,
			                const std::string &gs = "",
			                const std::string &ts = "",
			                BlendMode::List blendmode = BlendMode::Replace,
			                bool depthwrite = true,
			                DepthTest::List depthtest = DepthTest::Less);
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
			void prepareShaders(const std::string &flags);

			/**
			 * Returns a shader instance for a context, taking a certain flag
			 * set into account.
			 * @param context Shader context.
			 * @param flags Flag bitset as returned by getFlags().
			 * @return Shader combination or 0 if no shader could be created.
			 * @todo This (and other functions here) needs to be made threadsafe.
			 */
			ShaderCombination *getCombination(unsigned int context,
			                                  unsigned int flags);

			virtual const char *getType()
			{
				return "Shader";
			}

			virtual bool load();
			virtual void upload(void *data);

			virtual void compileCombination(ShaderCombination *combination) = 0;
			virtual void deleteCombination(ShaderCombination *combination) = 0;

			struct Sampler
			{
				std::string name;
				Texture::Ptr deftexture;
				TextureType::List type;
				unsigned int texunit;
				unsigned int flags;
			};
			struct Uniform
			{
				std::string name;
				unsigned int size;
				float defvalue[16];
			};
			struct ShaderInfo
			{
				std::vector<Sampler> samplers;
				std::vector<Uniform> uniforms;
				std::vector<unsigned int> attribs;
			};
			ShaderInfo *getUploadedData();

			typedef core::SharedPointer<Shader> Ptr;
		protected:
			virtual void *getUploadData();
		private:
			bool resolveIncludes(const std::string &text,
			                     std::string &output,
			                     const std::string &directory);

			struct Context
			{
				unsigned int name;
				std::string vs;
				std::string fs;
				std::string gs;
				std::string ts;
				BlendMode::List blendmode;
				bool depthwrite;
				DepthTest::List depthtest;
				std::vector<ShaderCombination::Ptr> combinations;
			};

			std::map<std::string, std::string> texts;
			std::vector<std::string> compilerflags;
			unsigned int flagdefaults;

			std::vector<Context> contexts;

			ShaderInfo currentinfo;
		protected:
			ShaderInfo uploadedinfo;
	};
}
}

#endif
