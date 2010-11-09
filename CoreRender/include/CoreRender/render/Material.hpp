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

#ifndef _CORERENDER_RENDER_MATERIAL_HPP_INCLUDED_
#define _CORERENDER_RENDER_MATERIAL_HPP_INCLUDED_

#include "Shader.hpp"
#include "Texture.hpp"
#include "UniformData.hpp"

namespace cr
{
namespace render
{
	/**
	 * Material definition which contains a shader text, flags, uniform values
	 * and textures.
	 */
	class Material : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Material(UploadManager &uploadmgr,
			         res::ResourceManager *rmgr,
			         const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Material();

			/**
			 * Sets the shader text of the material.
			 * @param shader New shader text resource.
			 */
			void setShader(Shader::Ptr shader);
			/**
			 * Returns the shader text used for this material.
			 * @return Shader text resource.
			 */
			Shader::Ptr getShader();

			/**
			 * Sets a shader flag string. For the format, see
			 * ShaderText::getFlags().
			 * @param flags Shader flag string.
			 */
			void setShaderFlags(const std::string &flags)
			{
				shaderflags = flags;
				if (shader)
				{
					shader->getFlags(flags, shaderflagmask, shaderflagvalue);
				}
			}
			/**
			 * Returns a shader flag string.
			 */
			std::string getShaderFlags()
			{
				return shaderflags;
			}
			/**
			 * Returns the shader flags as a bitset.
			 * @see Shader::getFlags()
			 */
			void getShaderFlags(unsigned int &flagmask,
			                    unsigned int &flagvalue)
			{
				flagmask = shaderflagmask;
				flagvalue = shaderflagvalue;
			}

			/**
			 * Material texture info.
			 */
			struct TextureInfo
			{
				/**
				 * Texture to be used for the sampler.
				 * @todo Should use NameRegistry.
				 */
				Texture::Ptr texture;
				/**
				 * Sampler name.
				 */
				std::string name;
			};

			/**
			 * Adds a texture entry.
			 * @param name Sampler name.
			 * @param texture Texture resource to be bound to the sampler.
			 */
			void addTexture(const std::string name, Texture::Ptr texture);
			/**
			 * Returns the texture bound to a certain sampler.
			 * @param name Sampler name.
			 * @return Texture bound to the sampler.
			 */
			Texture::Ptr getTexture(const std::string &name);
			/**
			 * Returns all textures used for this material.
			 */
			const std::vector<TextureInfo> &getTextures();

			/**
			 * Returns the uniform data used for this material. The uniform
			 * values are applied after the default shader text values, but
			 * before those specified by the user in RenderJob.
			 */
			UniformData &getUniformData()
			{
				return uniforms;
			}

			virtual bool load();

			virtual bool waitForLoading(bool recursive,
			                            bool highpriority = false);

			virtual const char *getType()
			{
				return "Material";
			}

			struct TextureList
			{
				TextureInfo *textures;
				unsigned int texturecount;
			};
			TextureList *getUploadedTextures()
			{
				return uploadeddata;
			}
			virtual void upload(void *data);


			typedef core::SharedPointer<Material> Ptr;
		protected:
			virtual void *getUploadData();
		private:
			Shader::Ptr shader;
			std::string shaderflags;
			unsigned int shaderflagmask;
			unsigned int shaderflagvalue;

			std::vector<TextureInfo> textures;
			TextureList *uploadeddata;

			UniformData uniforms;
	};
}
}

#endif
