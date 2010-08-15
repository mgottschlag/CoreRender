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

#include "RenderWindowSDL.hpp"

#include <SDL.h>
#include <SDL_syswm.h>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderWindowSDL::RenderWindowSDL() : initialized(false)
	{
		
	}
	RenderWindowSDL::~RenderWindowSDL()
	{
		if (initialized)
		{
			lockDisplay();
			glXMakeCurrent(display, window, primary);
			unlockDisplay();
			SDL_Quit();
		}
	}

	bool RenderWindowSDL::open(unsigned int width,
	                           unsigned int height,
	                           bool fullscreen)
	{
		// Create render window
		if(SDL_Init(SDL_INIT_VIDEO) != 0)
		{
			return false;
		}
		SDL_WM_SetCaption("CoreRender", "CoreRender");
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		if (!SDL_SetVideoMode(width,
		                      height,
		                      32,
		                      SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0)))
		{
			SDL_Quit();
			return false;
		}
		// Get window/context info
		SDL_SysWMinfo wminfo;
		SDL_VERSION(&wminfo.version);
		if (SDL_GetWMInfo(&wminfo) == -1)
		{
			SDL_Quit();
			return false;
		}
#if defined(CORERENDER_UNIX)
		if (wminfo.subsystem != SDL_SYSWM_X11)
		{
			SDL_Quit();
			return false;
		}
		//window = wminfo.info.x11.window;
		//display = wminfo.info.x11.gfxdisplay;
		window = glXGetCurrentDrawable();
		display = glXGetCurrentDisplay();
		primary = glXGetCurrentContext();
		lockDisplay = wminfo.info.x11.lock_func;
		unlockDisplay = wminfo.info.x11.unlock_func;
		// Deactivate context (we might bind it in another thread)
		lockDisplay();
		glXMakeCurrent(display, None, NULL);
		unlockDisplay();
#elif defined(CORERENDER_WINDOWS)
	#error Not implemented!
#else
	#error Not implemented!
#endif
		initialized = true;
		return true;
	}
	bool RenderWindowSDL::resize(unsigned int width,
	                             unsigned int height,
	                             bool fullscreen)
	{
#if defined(CORERENDER_UNIX)
		// This only works under linux
		if (!SDL_SetVideoMode(width,
		                      height,
		                      32,
		                      SDL_OPENGL | (fullscreen ? SDL_FULLSCREEN : 0)))
		{
			// TODO: This case might destroy the OpenGL context!
			return false;
		}
		return true;
#else
		// TODO
		return false;
#endif
	}
}
}
}
