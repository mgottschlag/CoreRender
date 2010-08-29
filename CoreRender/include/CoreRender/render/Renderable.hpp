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

#ifndef _CORERENDER_RENDER_RENDERABLE_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERABLE_HPP_INCLUDED_

namespace cr
{
namespace render
{
	class RenderJob;

	/**
	 * Renderable object which consists of one or more render jobs. This class
	 * can be subclassed to simplify rendering for a certain object type, e.g.
	 * ModelRenderable to render models.
	 */
	class Renderable
	{
		public:
			/**
			 * Constructor.
			 */
			Renderable()
			{
			}
			/**
			 * Destructor.
			 */
			virtual ~Renderable()
			{
			}

			/**
			 * Is called before any calls to getJob(). Has to return the number
			 * of batches which shall be rendered. Here the renderable object
			 * can also dynamically prepare render jobs for rendering. This is
			 * called by Pipeline::submit().
			 * @return Number of render jobs available.
			 */
			virtual unsigned int beginRendering() = 0;
			/**
			 * Returns a rendering job to be rendered. Pipeline::submit() calls
			 * this for all indices from 0 to one less than the number returned
			 * by beginRendering().
			 * @return index Index of the render job.
			 */
			virtual RenderJob *getJob(unsigned int index) = 0;
			/**
			 * Is called by Pipeline::submit() after all calls to getJob().
			 * Until this is called, the pointer returned by getJob() have to
			 * be valid, here they can be destroyed.
			 */
			virtual void endRendering()
			{
			}
		private:
	};
}
}

#endif
