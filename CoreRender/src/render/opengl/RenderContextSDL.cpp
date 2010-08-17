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

#include "RenderContextSDL.hpp"
#include "CoreRender/render/GraphicsEngine.hpp"
#include "KeyCodeSDL.hpp"

#include <SDL.h>
#include <iostream>

namespace cr
{
namespace render
{
namespace opengl
{
	RenderContextSDL::RenderContextSDL() : initialized(false), primary(false)
	{
	}
	RenderContextSDL::~RenderContextSDL()
	{
		if (initialized)
		{
#if defined(CORERENDER_UNIX)
			// TODO
#elif defined(CORERENDER_WINDOWS)
			if (!primary)
				wglDeleteContext(context);
#endif
		}
	}

	bool RenderContextSDL::create(unsigned int width,
	                              unsigned int height,
	                              bool fullscreen)
	{
		tbb::spin_mutex::scoped_lock lock(sdlmutex);
		// Open render window
		window = new RenderWindowSDL();
		if (!window->open(width, height, fullscreen))
		{
			window = 0;
			return false;
		}
		// Fetch context
#if defined(CORERENDER_UNIX)
		display = window->display;
		x11window = window->window;
		context = window->primary;
		lockDisplay = window->lockDisplay;
		unlockDisplay = window->unlockDisplay;
#elif defined(CORERENDER_WINDOWS)
		hdc = window->hdc;
		context = window->primary;
#endif
		primary = true;
		return true;
	}

	bool RenderContextSDL::resize(unsigned int width,
	                              unsigned int height,
	                              bool fullscreen)
	{
		tbb::spin_mutex::scoped_lock lock(sdlmutex);
		return window->resize(width, height, fullscreen);
	}

	void RenderContextSDL::makeCurrent(bool current)
	{
#if defined(CORERENDER_UNIX)
		lockDisplay();
		if (current)
		{
			// TODO: Check whether the context already is bound?
			glXMakeCurrent(display, x11window, context);
		}
		else
			glXMakeCurrent(display, None, NULL);
		unlockDisplay();
#elif defined(CORERENDER_WINDOWS)
		if (current)
		{
			// TODO: Check whether the context already is bound?
			wglMakeCurrent(hdc, context);
		}
		else
			wglMakeCurrent(hdc, NULL);
#endif
	}
	void RenderContextSDL::swapBuffers()
	{
		tbb::spin_mutex::scoped_lock lock(sdlmutex);
#if defined(CORERENDER_UNIX)
		lockDisplay();
		glXSwapBuffers(display, x11window);
		unlockDisplay();
#elif defined(CORERENDER_WINDOWS)
		SwapBuffers(hdc);
#endif
	}

	RenderContext::Ptr RenderContextSDL::clone()
	{
		tbb::spin_mutex::scoped_lock lock(sdlmutex);
#if defined(CORERENDER_UNIX)
		// Get visual info
		XWindowAttributes attrib;
		if (!XGetWindowAttributes(display, x11window, &attrib))
			return 0;
		XVisualInfo visualtpl;
		for (int i = 0; i < XScreenCount(display); i++)
		{
			if (XRootWindow(display, i) == attrib.root)
				visualtpl.screen = i;
		}
		visualtpl.visualid = XVisualIDFromVisual(attrib.visual);
		int visualcount;
		XVisualInfo *visual = XGetVisualInfo(display,
		                                     VisualScreenMask|VisualIDMask,
		                                     &visualtpl,
		                                     &visualcount);
		if (!visual)
			return 0;

		// Create context
		GLXContext newglxctx = glXCreateContext(display, visual, context, True);
		XFree(visual);
		if (!newglxctx)
			return 0;
		// Create SDL context class
		RenderContextSDL::Ptr newctx = new RenderContextSDL();
		newctx->window = window;
		newctx->initialized = true;
		newctx->primary = false;
		newctx->display = display;
		newctx->x11window = x11window;
		newctx->context = newglxctx;
		newctx->lockDisplay = lockDisplay;
		newctx->unlockDisplay = unlockDisplay;
		return newctx;
#elif defined(CORERENDER_WINDOWS)
		HGLRC newhglrc = wglCreateContext(hdc);
		if (!newhglrc)
			return 0;
		RenderContextSDL::Ptr newctx = new RenderContextSDL();
		newctx->hdc = hdc;
		newctx->context = newhglrc;
		wglShareLists(newctx->context, context);
		return newctx;
#else
		return 0;
#endif
	}

	void RenderContextSDL::update(GraphicsEngine *inputreceiver)
	{
		if (!primary)
		{
			tbb::spin_mutex::scoped_lock lock(sdlmutex);
#if defined(CORERENDER_UNIX)
			lockDisplay();
#endif
			// Poll events
			SDL_Event event;
			while (SDL_PollEvent(&event))
			{
				switch (event.type)
				{
					case SDL_QUIT:
					{
						InputEvent inputevent;
						inputevent.type = InputEventType::WindowClosed;
						inputreceiver->injectInput(inputevent);
						break;
					}
					case SDL_MOUSEMOTION:
					{
						InputEvent inputevent;
						inputevent.type = InputEventType::MouseMoved;
						inputevent.mouse.dx = event.motion.xrel;
						inputevent.mouse.dy = event.motion.yrel;
						inputevent.mouse.x = event.motion.x;
						inputevent.mouse.y = event.motion.y;
						inputreceiver->injectInput(inputevent);
						break;
					}
					case SDL_MOUSEBUTTONDOWN:
					case SDL_MOUSEBUTTONUP:
					{
						if (event.button.button == SDL_BUTTON_WHEELUP
							|| event.button.button == SDL_BUTTON_WHEELDOWN)
						{
							// TODO
							break;
						}
						InputEvent inputevent;
						if (event.button.type == SDL_MOUSEBUTTONDOWN)
							inputevent.type = InputEventType::MouseDown;
						else
							inputevent.type = InputEventType::MouseUp;
						inputevent.mouse.dx = inputevent.mouse.dy = 0;
						switch (event.button.button)
						{
							case SDL_BUTTON_LEFT:
								inputevent.mouse.button = 0;
								break;
							case SDL_BUTTON_RIGHT:
								inputevent.mouse.button = 1;
								break;
							case SDL_BUTTON_MIDDLE:
								inputevent.mouse.button = 2;
								break;
							default:
								inputevent.mouse.button = 0;
								break;
						}
						inputevent.mouse.x = event.button.x;
						inputevent.mouse.y = event.button.y;
						inputreceiver->injectInput(inputevent);
						break;
					}
					case SDL_KEYDOWN:
					case SDL_KEYUP:
					{
						InputEvent inputevent;
						if (event.type == SDL_KEYDOWN)
							inputevent.type = InputEventType::KeyDown;
						else
							inputevent.type = InputEventType::KeyUp;
						inputevent.keyboard.key = translateSDLKey(event.key.keysym.sym);
						if (inputevent.keyboard.key == KeyCode::Invalid)
						{
							inputreceiver->getLog()->warning("SDL: Invalid key pressed: %d",
							                                 event.key.keysym.sym);
							break;
						}
						inputreceiver->injectInput(inputevent);
						if (event.type == SDL_KEYDOWN)
						{
							// Also inject char event
							inputevent.type = InputEventType::CharTyped;
							inputevent.chartyped.unicode = event.key.keysym.unicode;
							inputreceiver->injectInput(inputevent);
						}
						break;
					}
					default:
						// TODO: Joystick events, visibility events
						break;
				}
			}
#if defined(CORERENDER_UNIX)
			unlockDisplay();
#endif
		}
	}

	tbb::spin_mutex RenderContextSDL::sdlmutex;
}
}
}
