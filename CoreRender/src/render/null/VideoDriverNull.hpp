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

#ifndef _CORERENDER_RENDER_NULL_VIDEODRIVER_HPP_INCLUDED_
#define _CORERENDER_RENDER_NULL_VIDEODRIVER_HPP_INCLUDED_

#include "../VideoDriver.hpp"
#include "RenderCapsNull.hpp"
#include "IndexBufferNull.hpp"
#include "VertexBufferNull.hpp"
#include "ShaderNull.hpp"
#include "FrameBufferNull.hpp"
#include "Texture2DNull.hpp"

namespace cr
{
namespace render
{
namespace null
{
	class VideoDriverNull : public VideoDriver
	{
		public:
			VideoDriverNull()
			{
			}
			virtual ~VideoDriverNull()
			{
			}

			virtual bool init()
			{
				return true;
			}
			virtual bool shutdown()
			{
				return true;
			}

			virtual Texture2D::Ptr createTexture2D(Renderer *renderer,
			                                       res::ResourceManager *rmgr,
			                                       const std::string &name)
			{
				return new Texture2DNull(renderer, rmgr, name);
			}
			virtual IndexBuffer::Ptr createIndexBuffer(Renderer *renderer,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name)
			{
				return new IndexBufferNull(renderer, rmgr, name);
			}
			virtual VertexBuffer::Ptr createVertexBuffer(Renderer *renderer,
			                                             res::ResourceManager *rmgr,
			                                             const std::string &name)
			{
				return new VertexBufferNull(renderer, rmgr, name);
			}
			virtual Shader::Ptr createShader(Renderer *renderer,
			                                 res::ResourceManager *rmgr,
			                                 const std::string &name)
			{
				return new ShaderNull(renderer, rmgr, name);
			}
			virtual FrameBuffer::Ptr createFrameBuffer(Renderer *renderer,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name)
			{
				return new FrameBufferNull(renderer, rmgr, name);
			}


			virtual void setRenderTarget(const RenderTargetInfo *target)
			{
			}
			virtual void setViewport(unsigned int x,
			                         unsigned int y,
			                         unsigned int width,
			                         unsigned int height)
			{
			}
			virtual void clear(bool colorbuffer,
			                   bool zbuffer,
			                   core::Color color = core::Color(0, 0, 0, 0),
			                   float depth = 0.0f)
			{
			}

			virtual void draw(RenderBatch *batch)
			{
				// Increase polygon/batch counters
				getStats().increaseBatchCount(1);
				getStats().increasePolygonCount((batch->endindex - batch->startindex) / 3);
			}

			virtual void endFrame()
			{
			}

			virtual VideoDriverType::List getType()
			{
				return VideoDriverType::Null;
			}
			virtual const RenderCaps &getCaps()
			{
				return caps;
			}
		private:
			RenderCapsNull caps;
	};

}
}
}

#endif
