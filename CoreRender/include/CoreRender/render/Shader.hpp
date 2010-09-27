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
#include "../core/HashMap.hpp"
#include "DefaultUniform.hpp"
#include "DepthTest.hpp"
#include "BlendMode.hpp"

namespace cr
{
namespace render
{
	class ShaderText;

	/**
	 * Resource containing a single shader instance.
	 *
	 * This class should be created via ShaderText::getShader().
	 */
	class Shader : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param renderer Renderer to be used with this shader.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Shader(Renderer *renderer,
			       res::ResourceManager *rmgr,
			       const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Shader();

			/**
			 * Sets the blend mode for this shader
			 */
			void setBlendMode(BlendMode::List mode)
			{
				blendmode = mode;
			}
			/**
			 * Get the blend mode for this shader
			 */
			BlendMode::List getBlendMode()
			{
				return blendmode;
			}
			/**
			 * Sets whether z writes are enabled.
			 */
			void setDepthWrite(bool depthwrite)
			{
				this->depthwrite = depthwrite;
			}
			/**
			 * Returns whether z writes are enabled.
			 */
			bool getDepthWrite()
			{
				return depthwrite;
			}
			/**
			 * Sets whether and how the depth test is performed.
			 */
			void setDepthTest(DepthTest::List depthtest)
			{
				this->depthtest = depthtest;
			}
			/**
			 * Returns whether and how the depth test is performed.
			 */
			DepthTest::List getDepthTest()
			{
				return depthtest;
			}
			
			/**
			 * Sets the vertex shader text for this shader.
			 * @param vs Vertex shader content.
			 */
			void setVertexShader(const std::string &vs);
			/**
			 * Sets the fragment shader text for this shader.
			 * @param fs Fragment shader content.
			 */
			void setFragmentShader(const std::string &fs);
			/**
			 * Sets the geometry shader text for this shader.
			 * @param gs Geometry shader content.
			 */
			void setGeometryShader(const std::string &gs);
			/**
			 * Sets the tesselation shader text for this shader.
			 * @param ts Tesselation shader content.
			 */
			void setTesselationShader(const std::string &ts);

			/**
			 * Adds an attrib name to the shader. Only attribs added like this
			 * get valid shader handles.
			 * @param name Attrib name.
			 */
			void addAttrib(const std::string &name);
			/**
			 * Returns the shader handle to an attrib.
			 * @param name Attrib name.
			 * @return Shader handle.
			 */
			int getAttrib(const std::string &name);
			/**
			 * Adds a uniform name to the shader. Only uniforms added like this
			 * get valid shader handles.
			 * @param name Uniform name.
			 */
			void addUniform(const std::string &name);
			/**
			 * Returns the shader handle to a uniform.
			 * @param name Uniform name.
			 * @return Shader handle.
			 */
			int getUniform(const std::string &name);
			/**
			 * Adds a sampler name to the shader. Only textures added like this
			 * get valid shader handles.
			 * @param name Sampler name.
			 */
			void addTexture(const std::string &name);
			/**
			 * Returns the shader handle to a texture.
			 * @param name Sampler name.
			 * @return Shader handle.
			 */
			int getTexture(const std::string &name);

			/**
			 * Updates the shader. This registers the shader for reupload.
			 * This is called by ShaderText::getShader().
			 */
			void updateShader();

			virtual void uploadShader()
			{
			}

			/**
			 * Returns the handle to this shader. This usually is the OpenGL
			 * program handle.
			 * @return Handle to the shader.
			 */
			int getHandle()
			{
				return handle;
			}
			virtual const char *getType()
			{
				return "Shader";
			}

			/**
			 * Sets the shader text resource this shader belongs to. This is
			 * called by ShaderText::getShader().
			 * @param text Shader text resource.
			 */
			void setShaderText(ShaderText *text)
			{
				this->text = text;
			}
			/**
			 * Returns the shader text this shader belongs to.
			 * @return Shader text resource.
			 */
			ShaderText *getShaderText()
			{
				return text;
			}

			const DefaultUniformLocations &getDefaultUniformLocations()
			{
				return defaultuniforms;
			}

			typedef core::SharedPointer<Shader> Ptr;
		protected:
			int handle;
			int oldhandle;

			ShaderText *text;

			tbb::spin_mutex textmutex;
			std::string vs;
			std::string fs;
			std::string gs;
			std::string ts;
			BlendMode::List blendmode;
			bool depthwrite;
			DepthTest::List depthtest;

			typedef core::HashMap<std::string, int>::Type HandleMap;
			HandleMap attribs;
			HandleMap uniforms;
			HandleMap textures;

			DefaultUniformLocations defaultuniforms;
	};
}
}

#endif
