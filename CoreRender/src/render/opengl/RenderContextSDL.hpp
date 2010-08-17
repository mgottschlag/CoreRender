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

#ifndef _CORERENDER_RENDER_OPENGL_RENDERCONTEXTSDL_HPP_INCLUDED_
#define _CORERENDER_RENDER_OPENGL_RENDERCONTEXTSDL_HPP_INCLUDED_

#include "CoreRender/render/RenderContextOpenGL.hpp"
#include "RenderWindowSDL.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace render
{
namespace opengl
{
	class RenderContextSDL : public RenderContextOpenGL
	{
		public:
			RenderContextSDL();
			virtual ~RenderContextSDL();

			bool create(unsigned int width,
			            unsigned int height,
			            bool fullscreen);

			virtual bool resize(unsigned int width,
			                    unsigned int height,
			                    bool fullscreen);

			virtual void makeCurrent(bool current = true);
			virtual void swapBuffers();

			virtual RenderContext::Ptr clone();

			virtual void update(GraphicsEngine *inputreceiver);

			typedef core::SharedPointer<RenderContextSDL> Ptr;
		private:
			RenderWindowSDL::Ptr window;
			bool initialized;
			bool primary;
#if defined(CORERENDER_UNIX)
			Display *display;
			Window x11window;
			GLXContext context;
			void (*lockDisplay)();
			void (*unlockDisplay)();
#elif defined(CORERENDER_WINDOWS)
			HDC hdc;
			HGLRC context;
#endif

			static tbb::spin_mutex sdlmutex;
	};
}
}
}

#endif
