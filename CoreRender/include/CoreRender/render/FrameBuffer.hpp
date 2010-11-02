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

#ifndef _CORERENDER_RENDER_FRAMEBUFFER_HPP_INCLUDED_
#define _CORERENDER_RENDER_FRAMEBUFFER_HPP_INCLUDED_

#include "RenderResource.hpp"

namespace cr
{
namespace render
{
	/**
	 * Resource describing the output format of a render-to-texture operation.
	 * This can be used by multiple render targets to reduce state switches in
	 * the video driver. A render target can have one depth buffer (either
	 * rendering the depth data to a texture or to an unnamed render buffer)
	 * and many color buffers. One color buffer shall always have the same type
	 * in all render targets using the frame buffer. All textures attached to
	 * the frame buffer shall have the same size as the frame buffer.
	 */
	class FrameBuffer : public RenderResource
	{
		public:
			/**
			 * Constructor.
			 * @param renderer Renderer to be used with this framebuffer.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			FrameBuffer(UploadManager &uploadmgr,
			            res::ResourceManager *rmgr,
			            const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~FrameBuffer();

			/**
			 * Sets the size of the framebuffer and whether a depth buffer shall
			 * be created. The depth buffer can be overridden by a depth texture
			 * via RenderTarget::setDepthBuffer(). All attached textures
			 * must have the same size as the frame buffer.
			 * @param width Width of the frame buffer.
			 * @param height Height of the frame buffer.
			 * @param hasdepthbuffer If true, a depth buffer is created and used
			 * if no depth texture was attached.
			 */
			void setSize(unsigned int width,
			             unsigned int height,
			             bool hasdepthbuffer);
			/**
			 * Returns the width of the frame buffer.
			 * @return Width.
			 */
			unsigned int getWidth();
			/**
			 * Returns the height of the frame buffer.
			 * @return Height.
			 */
			unsigned int getHeight();
			/**
			 * Returns whether this render target has a default depth buffer.
			 * @return Default depth buffer
			 */
			bool hasDepthBuffer();

			virtual const char *getType()
			{
				return "FrameBuffer";
			}

			struct Configuration
			{
				unsigned int handle;
				unsigned int depthbuffer;
				unsigned int defaultdepthbuffer;
				std::vector<unsigned int> colorbuffers;
			};

			/**
			 * Returns the configuration of the framebuffer. This is used to
			 * track the textures attached to the frame buffer. This must only
			 * be called by VideoDriver, do not call this directly.
			 * @return Framebuffer configuration.
			 */
			Configuration &getConfiguration()
			{
				return config;
			}

			typedef core::SharedPointer<FrameBuffer> Ptr;
		protected:
			Configuration config;
		private:
			unsigned int width;
			unsigned int height;
			bool depthbuffer;
			tbb::spin_mutex mutex;
	};
}
}

#endif
