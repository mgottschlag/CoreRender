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

#ifndef _CORERENDER_RENDER_OPENGL_VIDEODRIVER_HPP_INCLUDED_
#define _CORERENDER_RENDER_OPENGL_VIDEODRIVER_HPP_INCLUDED_

#include "CoreRender/render/VideoDriver.hpp"
#include "RenderCapsOpenGL.hpp"
#include "CoreRender/core/Log.hpp"

namespace cr
{
namespace render
{
namespace opengl
{
	class VideoDriverOpenGL : public VideoDriver
	{
		public:
			VideoDriverOpenGL(core::Log::Ptr log);
			virtual ~VideoDriverOpenGL();

			virtual bool init();
			virtual bool shutdown();

			virtual Texture2D::Ptr createTexture2D(Renderer *renderer,
			                                       res::ResourceManager *rmgr,
			                                       const std::string &name);
			virtual IndexBuffer::Ptr createIndexBuffer(Renderer *renderer,
			                                           res::ResourceManager *rmgr,
			                                           const std::string &name);
			virtual VertexBuffer::Ptr createVertexBuffer(Renderer *renderer,
			                                             res::ResourceManager *rmgr,
			                                             const std::string &name,
	                                                     VertexBufferUsage::List usage);

			virtual void setRenderTarget(int handle);
			virtual void clear(bool colorbuffer,
			                   bool zbuffer,
			                   core::Color color = core::Color(0, 0, 0, 0),
			                   float depth = 0.0f);

			virtual void draw(RenderBatch *batch);

			virtual void endFrame();

			virtual VideoDriverType::List getType()
			{
				return VideoDriverType::OpenGL;
			}
			virtual const RenderCaps &getCaps()
			{
				return caps;
			}
		private:
			RenderCapsOpenGL caps;

			core::Log::Ptr log;
	};

}
}
}

#endif
