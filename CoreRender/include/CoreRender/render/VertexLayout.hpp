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

#include "RenderObject.hpp"

#include <string>

namespace cr
{
	namespace render
	{
		/**
		 * Type of a single vertex attribute component.
		 */
		struct VertexElementType
		{
			enum List
			{
				Float,
				DoubleFloat,
				HalfFloat,
				Integer,
				UnsignedInteger,
				Short,
				UnsignedShort,
				Byte,
				UnsignedByte
			};
		};

		/**
		 * Single element within a vertex layout.
		 */
		struct VertexLayoutElement
		{
			/**
			 * Name of the attrib in shaders.
			 */
			unsigned int name;
			/**
			 * Vertex buffer slot of this element.
			 */
			unsigned int vbslot;
			/**
			 * Number of components of the vertex attrib.
			 */
			unsigned int components;
			/**
			 * Byte offset from the beginning of a vertex.
			 */
			unsigned int offset;
			/**
			 * Component type.
			 */
			VertexElementType::List type;
			/**
			 * Stride of the vertex data in the vertex slot.
			 */
			unsigned int stride;
			/**
			 * True if an integer value shall be normalized before being passed
			 * to the shader.
			 */
			bool normalize;
		};

		/**
		 * Class describing the layout of vertex attributes in memory. This
		 * class can describe any combination of structure-of-arrays or
		 * array-of-structure layouts, but is limited to one vertex buffer.
		 * The layout has a fixed number of elements which is set in the
		 * constructor. Every element in the layout has a name which is used for
		 * the attrib in shaders, a vertex buffer slot which relates to the
		 * index in a structure-of-arrays-layout, a byte offset to the beginning
		 * of one vertex in the slow, a number of vector components, a data type
		 * and the stride (byte offset from one vertex to the other).
		 *
		 * So, if you want to have a normal layout containing positions, normals
		 * and 2d texture coords, create the layout like this:
		 * @code
		 * VertexLayout::Ptr layout = new VertexLayout(3);
		 * layout->setElement(0, "pos", 0, 3, 0, VertexElementType::Float, 32);
		 * layout->setElement(1, "normal", 0, 3, 12, VertexElementType::Float, 32);
		 * layout->setElement(2, "texcoord0", 0, 2, 24, VertexElementType::Float, 32);
		 * @endcode
		 * If you wanted to put the texture coords into a second stream/slot
		 * instead of an interleaved layout, use this:
		 * @code
		 * VertexLayout::Ptr layout = new VertexLayout(3);
		 * layout->setElement(0, "pos", 0, 3, 0, VertexElementType::Float, 24);
		 * layout->setElement(1, "normal", 0, 3, 12, VertexElementType::Float, 24);
		 * layout->setElement(2, "texcoord0", 1, 2, 0, VertexElementType::Float, 8);
		 * @endcode
		 * @note Do not modify existing instances of this class while they are
		 * in use. Rather think about creating a new instance and using that.
		 * @note This class is a RenderObject even if it does not upload
		 * anything because it can only be deleted when the render thread is not
		 * using it anymore even if it does not store it in a SharedPointer.
		 */
		class VertexLayout : public RenderObject
		{
			public:
				/**
				 * Constructor.
				 * @param elemcount Number of elements in the layout.
				 */
				VertexLayout(UploadManager &uploadmgr, unsigned int elemcount)
					: RenderObject(uploadmgr), changecounter(0),
					elemcount(elemcount)
				{
					elements = new VertexLayoutElement[elemcount];
				}
				/**
				 * Dstructor.
				 */
				~VertexLayout()
				{
					delete[] elements;
				}

				/**
				 * Fills a single element with information about position/type
				 * in the vertex buffer.
				 * @param element Index of the element.
				 * @param name Name of the attrib in shaders.
				 * @param vbslot Vertex buffer slot this element is placed in.
				 * @param components Number of components for this element.
				 * @param offset Byte offset from the beginning of one vertex.
				 * @param type Memory type of a single component.
				 * @param stride Stride of one vertex in this slot.
				 */
				void setElement(unsigned int element,
				                unsigned int name,
				                unsigned int vbslot,
				                unsigned int components,
				                unsigned int offset,
				                VertexElementType::List type,
				                unsigned int stride = 0,
				                bool normalize = false)
				{
					elements[element].name = name;
					elements[element].vbslot = vbslot;
					elements[element].components = components;
					elements[element].offset = offset;
					elements[element].type = type;
					elements[element].stride = stride;
					elements[element].normalize = normalize;

					changecounter++;
				}

				/**
				 * Returns the element with the given index.
				 * @param index Element index.
				 * @return Element at the given index.
				 */
				VertexLayoutElement *getElement(unsigned int index)
				{
					return &elements[index];
				}
				/**
				 * Returns the element with the given name.
				 * @param name Element name.
				 * @return Element with the given name.
				 */
				VertexLayoutElement *getElementByName(unsigned int name)
				{
					for (unsigned int i = 0; i < elemcount; i++)
					{
						if (elements[i].name == name)
							return &elements[i];
					}
					return 0;
				}
				/**
				 * Returns the number of elements in the layout.
				 */
				unsigned int getElementCount()
				{
					return elemcount;
				}

				/**
				 * Returns the offset of a certain slot for the given number of
				 * vertices.
				 * @param vertexcount Number of vertices in the vertex buffer.
				 * @param slot Index of the slot.
				 * @return Byte offset of the slot.
				 */
				unsigned int getSlotOffset(unsigned int vertexcount,
				                           unsigned int slot)
				{
					// TODO
					return 0;
				}

				/**
				 * Returns the size in bytes for a certain component type.
				 * @param type Component type.
				 * @return Size of one component.
				 */
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
						case VertexElementType::UnsignedInteger:
							return 4;
						case VertexElementType::Short:
						case VertexElementType::UnsignedShort:
							return 2;
						case VertexElementType::UnsignedByte:
						case VertexElementType::Byte:
							return 1;
					}
				}

				virtual void upload(void *data)
				{
				}

				/**
				 * Returns the value of a counter which is incremented on every
				 * call to setElement().
				 *
				 * This can be used for conditional uploading of objects which
				 * use the layout.
				 * @return Change counter.
				 */
				unsigned int getChangeCounter()
				{
					return changecounter;
				}

				typedef core::SharedPointer<VertexLayout> Ptr;
			protected:
				virtual void *getUploadData()
				{
					return 0;
				}
			private:
				unsigned int changecounter;
				unsigned int elemcount;
				VertexLayoutElement *elements;
		};
	}
}

#endif
