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

#ifndef _CORERENDER_RENDER_VERTEXLAYOUT_HPP_INCLUDED_
#define _CORERENDER_RENDER_VERTEXLAYOUT_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"

#include <string>

namespace cr
{
	namespace render
	{
		struct VertexElementType
		{
			enum List
			{
				Float,
				DoubleFloat,
				HalfFloat,
				Integer,
				Short,
				Byte
			};
		};

		struct VertexLayoutElement
		{
			std::string name;
			unsigned int vbslot;
			unsigned int components;
			unsigned int offset;
			VertexElementType::List type;
			unsigned int stride;
		};

		/**
		 * @note Not thread-safe.
		 */
		class VertexLayout : public core::ReferenceCounted
		{
			public:
				VertexLayout(unsigned int elemcount) : elemcount(elemcount)
				{
					elements = new VertexLayoutElement[elemcount];
				}
				~VertexLayout()
				{
					delete[] elements;
				}

				void setElement(unsigned int element,
				                const std::string &name,
				                unsigned int vbslot,
				                unsigned int components,
				                unsigned int offset,
				                VertexElementType::List type,
				                unsigned int stride = 0)
				{
					elements[element].name = name;
					elements[element].vbslot = vbslot;
					elements[element].components = components;
					elements[element].offset = offset;
					elements[element].type = type;
					elements[element].stride = stride;
				}

				VertexLayoutElement *getElement(unsigned int index)
				{
					return &elements[index];
				}
				VertexLayoutElement *getElement(const std::string &name)
				{
					for (unsigned int i = 0; i < elemcount; i++)
					{
						if (elements[i].name == name)
							return &elements[i];
					}
					return 0;
				}
				unsigned int getElementCount()
				{
					return elemcount;
				}

				unsigned int getSlotOffset(unsigned int slot)
				{
					// TODO
					return 0;
				}

				static unsigned int getElementSize(VertexElementType::List type)
				{
					switch (type)
					{
						case VertexElementType::Float:
							return 4;
						case VertexElementType::DoubleFloat:
							return 8;
						case VertexElementType::HalfFloat:
							return 2;
						case VertexElementType::Integer:
							return 4;
						case VertexElementType::Short:
							return 2;
						case VertexElementType::Byte:
							return 1;
					}
				}

				typedef core::SharedPointer<VertexLayout> Ptr;
			private:
				unsigned int elemcount;
				VertexLayoutElement *elements;
		};
	}
}

#endif
