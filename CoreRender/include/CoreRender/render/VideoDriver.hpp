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

#include "VideoDriverType.hpp"
#include "RenderCaps.hpp"
#include "../core/Color.hpp"
#include "Texture2D.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.hpp"
#include "Shader.hpp"
#include "RenderStats.hpp"

namespace cr
{
namespace res
{
	class ResourceManager;
}
namespace render
{
	struct RenderBatch;
	struct RenderTargetInfo;
	class Renderer;

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
			 * Initializes the video driver. Has to be called when the primary
			 * render context is active.
			 * @return Returns false if initialization failed.
			 */
			virtual bool init() = 0;
			/**
			 * Terminates the video driver. Has to be called when the primary
			 * render context is active.
			 * @return Returns false if termination failed.
			 */
			virtual bool shutdown() = 0;

			/**
			 * Creates a Texture2D resource. This is usually called by the
			 * Texture2D resource factory.
			 */
			virtual Texture2D::Ptr createTexture2D(Renderer *renderer,
			                                       res::ResourceManager *rmgr,
			                                       const std::string &name) = 0;
			/**
			 * Creates a IndexBuffer resource. This is usually called by the
			 * IndexBuffer resource factory.
			 */
			virtual IndexBuffer::Ptr createIndexBuffer(Renderer *renderer,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name) = 0;
			/**
			 * Creates a VertexBuffer resource. This is usually called by the
			 * VertexBuffer resource factory.
			 */
			virtual VertexBuffer::Ptr createVertexBuffer(Renderer *renderer,
			                                             res::ResourceManager *rmgr,
			                                             const std::string &name) = 0;
			/**
			 * Creates a Shader resource. This is usually called by the
			 * Shader resource factory.
			 */
			virtual Shader::Ptr createShader(Renderer *renderer,
			                                 res::ResourceManager *rmgr,
			                                 const std::string &name) = 0;

			/**
			 * Sets a render target for following calls to draw() or clear() to
			 * render into.
			 * @param handle Handle of the render target. If this is -1, the
			 * driver renders to the screen.
			 */
			virtual void setRenderTarget(const RenderTargetInfo &target) = 0;
			/**
			 * Clears the current render target.
			 * @param colorbuffer If true, clears the color buffer.
			 * @param zbuffer If true, clears the depth buffer.
			 * @param color Color to set the color buffer to.
			 * @param depth Depth to set the depth buffer to.
			 */
			virtual void clear(bool colorbuffer,
			                   bool zbuffer,
			                   core::Color color = core::Color(0, 0, 0, 0),
			                   float depth = 1.0f) = 0;

			/**
			 * Draws a single batch.
			 * @param batch Batch to be drawn.
			 */
			virtual void draw(RenderBatch *batch) = 0;

			/**
			 * Called at the end of the frame. This cleans up currently bound
			 * resources.
			 */
			virtual void endFrame() = 0;

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
			 * Returns the render stats of the running frame.
			 */
			RenderStats &getStats()
			{
				return stats;
			}
		private:
			RenderStats stats;
	};
}
}

#endif
