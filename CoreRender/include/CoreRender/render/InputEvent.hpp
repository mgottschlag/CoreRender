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

#ifndef _CORERENDER_RENDER_INPUTEVENT_HPP_INCLUDED_
#define _CORERENDER_RENDER_INPUTEVENT_HPP_INCLUDED_

#include "KeyCode.hpp"

namespace cr
{
namespace render
{
	struct InputEventType
	{
		/**
		 * Basic event type.
		 */
		enum List
		{
			/**
			 * The user tried to close the window. If you ignore this event,
			 * the window will not be closed automatically. Usually you want to
			 * exit your program though if you receive this.
			 */
			WindowClosed,
			/**
			 * The user has pressed a key. Use InputEvent::keyboard for more
			 * information.
			 */
			KeyDown,
			/**
			 * The released has pressed a key. Use InputEvent::keyboard for more
			 * information.
			 */
			KeyUp,
			/**
			 * The user has typed a character. This is emitted additionally to
			 * KeyDown. Use InputEvent::chartyped for more information.
			 */
			CharTyped,
			/**
			 * The user has moved the mouse. Use InputEvent::mouse for more
			 * information.
			 */
			MouseMoved,
			/**
			 * The user has pressed a mouse button. Use InputEvent::mouse for
			 * more information.
			 */
			MouseDown,
			/**
			 * The user has released a mouse button. Use InputEvent::mouse for
			 * more information.
			 */
			MouseUp
		};
	};
	/**
	 * Keyboard event data.
	 */
	struct KeyboardEvent
	{
		/**
		 * Keycode of the key pressed/released.
		 */
		KeyCode::List key;
	};
	/**
	 * Character event data.
	 */
	struct CharEvent
	{
		/**
		 * Unicode value of the character typed.
		 */
		unsigned int unicode;
	};
	/**
	 * Mouse event data.
	 */
	struct MouseEvent
	{
		/**
		 * Absolute x position of the mouse in the render window.
		 */
		int x;
		/**
		 * Absolute y position of the mouse in the render window.
		 */
		int y;
		/**
		 * Absolute dx position of the mouse in the render window.
		 */
		int dx;
		/**
		 * Absolute dy position of the mouse in the render window.
		 */
		int dy;
		/**
		 * Mouse button which has been pressed/released. Does not contain any
		 * valid information if the event type is InputEventType::MouseMoved.
		 * @todo Make this an enum.
		 */
		int button;
	};
	/**
	 * Class containing a keyboard/mouse/window event.
	 */
	struct InputEvent
	{
		/**
		 * Constructor.
		 * @param type Type of the event.
		 */
		InputEvent(InputEventType::List type) : type(type)
		{
		}
		/**
		 * Destructor.
		 */
		InputEvent()
		{
		}

		/**
		 * Type of the event. This tells you which event struct (keyboard,
		 * mouse, chartyped) to read for event data.
		 */
		InputEventType::List type;
		union
		{
			/**
			 * Contains data about the event in case of a keyboard event.
			 */
			KeyboardEvent keyboard;
			/**
			 * Contains data about the event in case of a mouse event.
			 */
			MouseEvent mouse;
			/**
			 * Contains data if a unicode char was typed.
			 */
			CharEvent chartyped;
		};
	};
}
}

#endif
