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

#ifndef _CORERENDER_RENDER_KEYCODE_HPP_INCLUDED_
#define _CORERENDER_RENDER_KEYCODE_HPP_INCLUDED_

namespace cr
{
namespace render
{
	struct KeyCode
	{
		enum List
		{
			Invalid = 0x0,
			MinChar = 0,
			A = 'a',
			B = 'b',
			C = 'c',
			D = 'd',
			E = 'e',
			F = 'f',
			G = 'g',
			H = 'h',
			I = 'i',
			J = 'j',
			K = 'k',
			L = 'l',
			M = 'm',
			N = 'n',
			O = 'o',
			P = 'p',
			Q = 'q',
			R = 'r',
			S = 's',
			T = 't',
			U = 'u',
			V = 'v',
			W = 'w',
			X = 'x',
			Y = 'y',
			Z = 'z',
			Key1 = '1',
			Key2 = '2',
			Key3 = '3',
			Key4 = '4',
			Key5 = '5',
			Key6 = '6',
			Key7 = '7',
			Key8 = '8',
			Key9 = '9',
			Key0 = '0',
			Tab = '\t',
			Space = ' ',
			Plus = '+',
			Comma = ',',
			Period = '.',
			Minus = '-',
			Hash = '#',
			Slash = '/',
			Caret = '^',
			Exclaim = '!',
			DoubleQuote = '\"',
			Dollar = '$',
			Ampersand = '&',
			SingleQuote = '\'',
			LeftParen = '(',
			RightParen = ')',
			Asterisk = '*',
			Colon = ':',
			Semicolon = ';',
			Less = '<',
			Equals = '=',
			Greater = '>',
			Question = '?',
			At = '@',
			LeftBracket = '[',
			RightBracket = ']',
			Underscore = '_',
			BackQuote = '`',
			BackSlash = '\\',
			MaxChar = 0x100,
			Return,
			Escape,
			Backspace,
			Insert,
			Delete,
			Home,
			End,
			Clear,
			Mode,
			Compose,
			Break,
			SysRq,
			Pause,
			PageUp,
			PageDown,
			LControl,
			RControl,
			LShift,
			RShift,
			LAlt,
			RAlt,
			LMeta,
			RMeta,
			LSuper,
			RSuper,
			AltGr,
			Menu,
			Help,
			Print,
			Power,
			Euro,
			Undo,
			Redo,
			Up,
			Down,
			Left,
			Right,
			NumLock,
			CapsLock,
			ScrollLock,
			F1,
			F2,
			F3,
			F4,
			F5,
			F6,
			F7,
			F8,
			F9,
			F10,
			F11,
			F12,
			F13,
			F14,
			F15,
			Keypad1,
			Keypad2,
			Keypad3,
			Keypad4,
			Keypad5,
			Keypad6,
			Keypad7,
			Keypad8,
			Keypad9,
			Keypad0,
			KeypadPeriod,
			KeypadPlus,
			KeypadMinus,
			KeypadMultiply,
			KeypadDivide,
			KeypadEnter,
			KeypadEquals
		};
	};
}
}

#endif
