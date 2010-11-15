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

#ifndef _CORERENDER_RENDER_VIDEODRIVER_HPP_INCLUDED_
#define _CORERENDER_RENDER_VIDEODRIVER_HPP_INCLUDED_

#include "CoreRender/render/VideoDriverType.hpp"
#include "CoreRender/render/RenderCaps.hpp"
#include "CoreRender/core/Color.hpp"
#include "CoreRender/render/Texture2D.hpp"
#include "CoreRender/render/IndexBuffer.hpp"
#include "CoreRender/render/VertexBuffer.hpp"
#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/RenderStats.hpp"
#include "CoreRender/render/FrameBuffer.hpp"

namespace cr
{
namespace res
{
	class ResourceManager;
}
namespace render
{
	struct RenderCommand;
	struct RenderTargetInfo;
	class UploadManager;
	struct RenderQueue;
	struct Batch;
	class Material;
	struct TextureBinding;

	/**
	 * Render system backend. This is used to create backend-specific classes
	 * (e.g. OpenGL resources for the OpenGL video driver) and to draw batches.
	 */
	class VideoDriver
	{
		public:
			/**
			 * Destructor.
			 */
			virtual ~VideoDriver()
			{
			}

			/**
			 * Initializes the video driver. Has to be called when the OpenGL
			 * context is active.
			 * @return Returns false if initialization failed.
			 */
			virtual bool init() = 0;
			/**
			 * Terminates the video driver. Has to be called when the OpenGL
			 * context is active.
			 * @return Returns false if termination failed.
			 */
			virtual bool shutdown() = 0;

			/**
			 * Creates a Texture2D resource. This is usually called by the
			 * Texture2D resource factory.
			 */
			virtual Texture2D::Ptr createTexture2D(UploadManager &uploadmgr,
			                                       res::ResourceManager *rmgr,
			                                       const std::string &name) = 0;
			/**
			 * Creates a IndexBuffer resource. This is usually called by the
			 * IndexBuffer resource factory.
			 */
			virtual IndexBuffer::Ptr createIndexBuffer(UploadManager &uploadmgr,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name) = 0;
			/**
			 * Creates a VertexBuffer resource. This is usually called by the
			 * VertexBuffer resource factory.
			 */
			virtual VertexBuffer::Ptr createVertexBuffer(UploadManager &uploadmgr,
			                                             res::ResourceManager *rmgr,
			                                             const std::string &name) = 0;
			/**
			 * Creates a Shader resource. This is usually called by the
			 * Shader resource factory.
			 */
			virtual Shader::Ptr createShader(UploadManager &uploadmgr,
			                                 res::ResourceManager *rmgr,
			                                 const std::string &name) = 0;
			/**
			 * Creates a frame buffer resource. This is usually called by the
			 * FrameBuffer resource factory.
			 */
			virtual FrameBuffer::Ptr createFrameBuffer(UploadManager &uploadmgr,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name) = 0;

			/**
			 * Sets a render target for following calls to draw() or clear() to
			 * render into.
			 * @param target Render target information. If this is 0, the driver
			 * renders to the screen.
			 */
			virtual void setRenderTarget(const RenderTargetInfo *target) = 0;
			/**
			 * Sets the viewport within the current render target to which the
			 * driver renders.
			 * @param x X position of the viewport.
			 * @param y Y position of the viewport.
			 * @param width X size of the viewport.
			 * @param height Y size of the viewport.
			 */
			virtual void setViewport(unsigned int x,
			                         unsigned int y,
			                         unsigned int width,
			                         unsigned int height) = 0;
			/**
			 * Clears the current render target.
			 * @param buffers Bitset containing the buffers which need to be
			 * cleared, with 0x1 being the back buffer, 0x2 the first color
			 * buffer, 0x4 the second etc.
			 * @param color Color to set the color buffers to.
			 * @param depth Depth to set the depth buffer to.
			 */
			virtual void clear(unsigned int buffers,
			                   float *color,
			                   float depth) = 0;

			/**
			 * Draws a single batch.
			 * @param batch Batch to be drawn.
			 */
			virtual void draw(Batch *batch) = 0;

			/**
			 * Draws a simple quad.
			 * @param vertices Four vertices, each one consisting of two floats
			 * for the position in view space (coordinates shall be in the range
			 * -1..1).
			 * @param shader Shader used for rendering the quad.
			 * @param material Material settings used for the quad.
			 */
			virtual void drawQuad(float *vertices,
			                      ShaderCombination *shader,
			                      Material *material) = 0;

			/**
			 * Called at the end of the frame. This cleans up currently bound
			 * resources.
			 */
			virtual void endFrame() = 0;

			virtual void setMatrices(math::Matrix4 projmat,
			                         math::Matrix4 viewmat) = 0;

			void bindTextures(TextureBinding *textures, unsigned int texturecount)
			{
				boundtextures = textures;
				this->texturecount = texturecount;
			}
			TextureBinding *getBoundTextures()
			{
				return boundtextures;
			}
			unsigned int getBoundTextureCount()
			{
				return texturecount;
			}

			/**
			 * Returns the type of this video driver.
			 */
			virtual VideoDriverType::List getType() = 0;
			/**
			 * Returns the capabilities of the video driver/GPU. This is only
			 * valid after init() has been called.
			 */
			virtual const RenderCaps &getCaps() = 0;

			/**
			 * Renders a chain of render commands.
			 */
			void executeCommands(RenderCommand *command);
			/**
			 * Executes a single render command.
			 */
			void executeCommand(RenderCommand *command);

			/**
			 * Returns the render stats of the running frame.
			 */
			RenderStats &getStats()
			{
				return stats;
			}
		private:
			void renderQueue(RenderQueue *queue);

			RenderStats stats;

			TextureBinding *boundtextures;
			unsigned int texturecount;
	};
}
}

#endif
