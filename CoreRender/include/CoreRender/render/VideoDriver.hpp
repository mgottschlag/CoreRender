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

namespace cr
{
namespace res
{
	class ResourceManager;
}
namespace render
{
	struct RenderBatch;
	class Renderer;

	class VideoDriver
	{
		public:
			virtual ~VideoDriver()
			{
			}

			virtual bool init() = 0;
			virtual bool shutdown() = 0;

			virtual Texture2D::Ptr createTexture2D(Renderer *renderer,
			                                       res::ResourceManager *rmgr,
			                                       const std::string &name) = 0;
			virtual IndexBuffer::Ptr createIndexBuffer(Renderer *renderer,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name) = 0;
			virtual VertexBuffer::Ptr createVertexBuffer(Renderer *renderer,
			                                             res::ResourceManager *rmgr,
			                                             const std::string &name,
			                                             VertexBufferUsage::List usage) = 0;
			virtual Shader::Ptr createShader(Renderer *renderer,
			                                 res::ResourceManager *rmgr,
			                                 const std::string &name) = 0;

			virtual void setRenderTarget(int handle) = 0;
			virtual void clear(bool colorbuffer,
			                   bool zbuffer,
			                   core::Color color = core::Color(0, 0, 0, 0),
			                   float depth = 1.0f) = 0;

			virtual void draw(RenderBatch *batch) = 0;

			virtual void endFrame() = 0;

			virtual VideoDriverType::List getType() = 0;
			virtual const RenderCaps &getCaps() = 0;
		private:
	};
}
}

#endif
