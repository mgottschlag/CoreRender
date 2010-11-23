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

#ifndef _CORERENDER_RENDER_RENDERTARGET_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERTARGET_HPP_INCLUDED_

#include "Texture.hpp"
#include "FrameBuffer.hpp"
#include "../res/Resource.hpp"

namespace cr
{
namespace core
{
	class MemoryPool;
}
namespace render
{
	struct RenderTargetInfo
	{
		FrameBuffer::Configuration *framebuffer;
		Texture *depthbuffer;
		unsigned int colorbuffercount;
		Texture **colorbuffers;
	};

	class RenderTarget : public res::Resource
	{
		public:
			RenderTarget(res::ResourceManager *rmgr,
			             const std::string &name);
			virtual ~RenderTarget();

			void setFrameBuffer(FrameBuffer::Ptr framebuffer);
			FrameBuffer::Ptr getFrameBuffer();

			void setDepthBuffer(Texture::Ptr texture);
			Texture::Ptr getDepthBuffer();

			void addColorBuffer(Texture::Ptr texture);
			Texture::Ptr getColorBuffer(unsigned int index);
			void removeColorBuffer(unsigned int index);
			unsigned int getColorBufferCount();

			void getRenderTargetInfo(RenderTargetInfo &info,
			                         core::MemoryPool *memory);

			virtual const char *getType()
			{
				return "RenderTarget";
			}

			typedef core::SharedPointer<RenderTarget> Ptr;
		private:
			FrameBuffer::Ptr framebuffer;
			std::vector<Texture::Ptr> colorbuffers;
			Texture::Ptr depthbuffer;
	};
}
}

#endif
