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

#ifndef _CORERENDER_SCENE_HEIGHTMAP_HPP_INCLUDED_
#define _CORERENDER_SCENE_HEIGHTMAP_HPP_INCLUDED_

#include "CoreRender/core/ReferenceCounted.hpp"
#include "CoreRender/core/FileSystem.hpp"

namespace cr
{
namespace scene
{
	/**
	 * Class which contains a 2d array of floats interpreted as heights.
	 */
	class HeightMap : public core::ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			HeightMap();
			/**
			 * Destructor.
			 */
			~HeightMap();

			/**
			 * Loads the height map from a file.
			 */
			bool load(core::FileSystem::Ptr fs, const std::string &filename);

			/**
			 * Returns the width of the height map.
			 */
			unsigned int getWidth()
			{
				return width;
			}
			/**
			 * Returns the height of the height map.
			 */
			unsigned int getHeight()
			{
				return height;
			}
			/**
			 * Returns the raw height map data.
			 */
			float *getData()
			{
				return data;
			}

			/**
			 * Returns the height at a certain point using linear interpolation.
			 */
			float getHeight(float x, float y);

			typedef core::SharedPointer<HeightMap> Ptr;
		private:
			float getEntry(unsigned int x, unsigned int y)
			{
				return data[y * width + x];
			}

			float *data;
			unsigned int width;
			unsigned int height;
	};
}
}
#endif
