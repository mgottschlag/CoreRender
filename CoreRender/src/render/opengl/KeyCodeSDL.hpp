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

#ifndef _CORERENDER_RENDER_KEYCODESDL_HPP_INCLUDED_
#define _CORERENDER_RENDER_KEYCODESDL_HPP_INCLUDED_

#include "CoreRender/render/KeyCode.hpp"

#include <SDL.h>

namespace cr
{
namespace render
{
namespace opengl
{
	static inline KeyCode::List translateSDLKey(SDLKey sdlkey)
	{
		switch (sdlkey)
		{
			case SDLK_BACKSPACE:
				return KeyCode::Backspace;
			case SDLK_TAB:
				return KeyCode::Tab;
			case SDLK_CLEAR:
				return KeyCode::Clear;
			case SDLK_RETURN:
				return KeyCode::Return;
			case SDLK_PAUSE:
				return KeyCode::Pause;
			case SDLK_ESCAPE:
				return KeyCode::Escape;
			case SDLK_SPACE:
				return KeyCode::Space;
			case SDLK_EXCLAIM:
				return KeyCode::Exclaim;
			case SDLK_QUOTEDBL:
				return KeyCode::DoubleQuote;
			case SDLK_HASH:
				return KeyCode::Hash;
			case SDLK_DOLLAR:
				return KeyCode::Dollar;
			case SDLK_AMPERSAND:
				return KeyCode::Ampersand;
			case SDLK_QUOTE:
				return KeyCode::SingleQuote;
			case SDLK_LEFTPAREN:
				return KeyCode::LeftParen;
			case SDLK_RIGHTPAREN:
				return KeyCode::RightParen;
			case SDLK_ASTERISK:
				return KeyCode::Asterisk;
			case SDLK_PLUS:
				return KeyCode::Plus;
			case SDLK_COMMA:
				return KeyCode::Comma;
			case SDLK_MINUS:
				return KeyCode::Minus;
			case SDLK_PERIOD:
				return KeyCode::Period;
			case SDLK_SLASH:
				return KeyCode::Slash;
			case SDLK_0:
				return KeyCode::Key0;
			case SDLK_1:
				return KeyCode::Key1;
			case SDLK_2:
				return KeyCode::Key2;
			case SDLK_3:
				return KeyCode::Key3;
			case SDLK_4:
				return KeyCode::Key4;
			case SDLK_5:
				return KeyCode::Key5;
			case SDLK_6:
				return KeyCode::Key6;
			case SDLK_7:
				return KeyCode::Key7;
			case SDLK_8:
				return KeyCode::Key8;
			case SDLK_9:
				return KeyCode::Key9;
			case SDLK_COLON:
				return KeyCode::Colon;
			case SDLK_SEMICOLON:
				return KeyCode::Semicolon;
			case SDLK_LESS:
				return KeyCode::Less;
			case SDLK_EQUALS:
				return KeyCode::Equals;
			case SDLK_GREATER:
				return KeyCode::Greater;
			case SDLK_QUESTION:
				return KeyCode::Question;
			case SDLK_AT:
				return KeyCode::At;
			case SDLK_LEFTBRACKET:
				return KeyCode::LeftBracket;
			case SDLK_BACKSLASH:
				return KeyCode::BackSlash;
			case SDLK_RIGHTBRACKET:
				return KeyCode::RightBracket;
			case SDLK_CARET:
				return KeyCode::Caret;
			case SDLK_UNDERSCORE:
				return KeyCode::Underscore;
			case SDLK_BACKQUOTE:
				return KeyCode::BackQuote;
			case SDLK_a:
				return KeyCode::A;
			case SDLK_b:
				return KeyCode::B;
			case SDLK_c:
				return KeyCode::C;
			case SDLK_d:
				return KeyCode::D;
			case SDLK_e:
				return KeyCode::E;
			case SDLK_f:
				return KeyCode::F;
			case SDLK_g:
				return KeyCode::G;
			case SDLK_h:
				return KeyCode::H;
			case SDLK_i:
				return KeyCode::I;
			case SDLK_j:
				return KeyCode::J;
			case SDLK_k:
				return KeyCode::K;
			case SDLK_l:
				return KeyCode::L;
			case SDLK_m:
				return KeyCode::M;
			case SDLK_n:
				return KeyCode::N;
			case SDLK_o:
				return KeyCode::O;
			case SDLK_p:
				return KeyCode::P;
			case SDLK_q:
				return KeyCode::Q;
			case SDLK_r:
				return KeyCode::R;
			case SDLK_s:
				return KeyCode::S;
			case SDLK_t:
				return KeyCode::T;
			case SDLK_u:
				return KeyCode::U;
			case SDLK_v:
				return KeyCode::V;
			case SDLK_w:
				return KeyCode::W;
			case SDLK_x:
				return KeyCode::X;
			case SDLK_y:
				return KeyCode::Y;
			case SDLK_z:
				return KeyCode::Z;
			case SDLK_DELETE:
				return KeyCode::Delete;
			case SDLK_KP0:
				return KeyCode::Keypad0;
			case SDLK_KP1:
				return KeyCode::Keypad1;
			case SDLK_KP2:
				return KeyCode::Keypad2;
			case SDLK_KP3:
				return KeyCode::Keypad3;
			case SDLK_KP4:
				return KeyCode::Keypad4;
			case SDLK_KP5:
				return KeyCode::Keypad5;
			case SDLK_KP6:
				return KeyCode::Keypad6;
			case SDLK_KP7:
				return KeyCode::Keypad7;
			case SDLK_KP8:
				return KeyCode::Keypad8;
			case SDLK_KP9:
				return KeyCode::Keypad9;
			case SDLK_KP_PERIOD:
				return KeyCode::KeypadPeriod;
			case SDLK_KP_DIVIDE:
				return KeyCode::KeypadDivide;
			case SDLK_KP_MULTIPLY:
				return KeyCode::KeypadMultiply;
			case SDLK_KP_MINUS:
				return KeyCode::KeypadMinus;
			case SDLK_KP_PLUS:
				return KeyCode::KeypadPlus;
			case SDLK_KP_ENTER:
				return KeyCode::KeypadEnter;
			case SDLK_KP_EQUALS:
				return KeyCode::KeypadEquals;
			case SDLK_UP:
				return KeyCode::Up;
			case SDLK_DOWN:
				return KeyCode::Down;
			case SDLK_RIGHT:
				return KeyCode::Right;
			case SDLK_LEFT:
				return KeyCode::Left;
			case SDLK_INSERT:
				return KeyCode::Insert;
			case SDLK_HOME:
				return KeyCode::Home;
			case SDLK_END:
				return KeyCode::End;
			case SDLK_PAGEUP:
				return KeyCode::PageUp;
			case SDLK_PAGEDOWN:
				return KeyCode::PageDown;
			case SDLK_F1:
				return KeyCode::F1;
			case SDLK_F2:
				return KeyCode::F2;
			case SDLK_F3:
				return KeyCode::F3;
			case SDLK_F4:
				return KeyCode::F4;
			case SDLK_F5:
				return KeyCode::F5;
			case SDLK_F6:
				return KeyCode::F6;
			case SDLK_F7:
				return KeyCode::F7;
			case SDLK_F8:
				return KeyCode::F8;
			case SDLK_F9:
				return KeyCode::F9;
			case SDLK_F10:
				return KeyCode::F10;
			case SDLK_F11:
				return KeyCode::F11;
			case SDLK_F12:
				return KeyCode::F12;
			case SDLK_F13:
				return KeyCode::F13;
			case SDLK_F14:
				return KeyCode::F14;
			case SDLK_F15:
				return KeyCode::F15;
			case SDLK_NUMLOCK:
				return KeyCode::NumLock;
			case SDLK_CAPSLOCK:
				return KeyCode::CapsLock;
			case SDLK_SCROLLOCK:
				return KeyCode::ScrollLock;
			case SDLK_RSHIFT:
				return KeyCode::RShift;
			case SDLK_LSHIFT:
				return KeyCode::LShift;
			case SDLK_RCTRL:
				return KeyCode::RControl;
			case SDLK_LCTRL:
				return KeyCode::LControl;
			case SDLK_RALT:
				return KeyCode::RAlt;
			case SDLK_LALT:
				return KeyCode::LAlt;
			case SDLK_RMETA:
				return KeyCode::RMeta;
			case SDLK_LMETA:
				return KeyCode::RMeta;
			case SDLK_LSUPER:
				return KeyCode::LSuper;
			case SDLK_RSUPER:
				return KeyCode::RSuper;
			case SDLK_MODE:
				return KeyCode::Mode;
			case SDLK_COMPOSE:
				return KeyCode::Compose;
			case SDLK_HELP:
				return KeyCode::Help;
			case SDLK_PRINT:
				return KeyCode::Print;
			case SDLK_SYSREQ:
				return KeyCode::SysRq;
			case SDLK_BREAK:
				return KeyCode::Break;
			case SDLK_MENU:
				return KeyCode::Menu;
			case SDLK_POWER:
				return KeyCode::Power;
			case SDLK_EURO:
				return KeyCode::Euro;
			case SDLK_UNDO:
				return KeyCode::Undo;
			default:
				return KeyCode::Invalid;
		}
	}
}
}
}

#endif
