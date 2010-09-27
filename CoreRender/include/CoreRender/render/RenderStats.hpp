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

#ifndef _CORERENDER_RENDER_RENDERSTATS_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERSTATS_HPP_INCLUDED_

#include "../math/StdInt.hpp"
#include "../core/Time.hpp"

namespace cr
{
namespace render
{
	/**
	 * Class containing some statistics about the last rendered frame.
	 */
	class RenderStats
	{
		public:
			/**
			 * Constructor.
			 */
			RenderStats()
				: polygons(0), batches(0), fps(0.0f),
				frametime(core::Duration::Seconds(0)),
				rendertime(core::Duration::Seconds(0)),
				waittime(core::Duration::Seconds(0))
			{
			}
			/**
			 * Copy constructor.
			 */
			RenderStats(const RenderStats &other)
			{
				polygons = other.polygons;
				batches = other.batches;
				fps = other.fps;
				frametime = other.frametime;
				rendertime = other.rendertime;
				waittime = other.waittime;
				framebegin = other.framebegin;
				renderbegin = other.renderbegin;
				frameend = other.frameend;
			}
			/**
			 * Destructor.
			 */
			~RenderStats()
			{
			}

			/**
			 * Returns the number of polygons (usually triangles) rendered.
			 */
			unsigned int getPolygonCount() const
			{
				return polygons;
			}
			/**
			 * Returns the number of calls to VideoDriver::draw(). These usually
			 * correspond to GPU drawing operations.
			 */
			unsigned int getBatchCount() const
			{
				return batches;
			}
			/**
			 * Returns the number of frames rendered per second. Note that this
			 * is only measured based on the time of one single frame, so this
			 * number will vary wildly.
			 */
			float getFPS() const
			{
				return fps;
			}
			/**
			 * Returns the time needed to for the frame. This includes both the
			 * time needed for rendering (getRenderTime()) and the time lost for
			 * syncronization with the main thread (getWaitTime()).
			 */
			core::Duration getFrameTime() const
			{
				return frametime;
			}
			/**
			 * Returns the time needed for only rendering.
			 */
			core::Duration getRenderTime() const
			{
				return rendertime;
			}
			/**
			 * Returns the time lost for waiting for the main thread.
			 */
			core::Duration getWaitTime() const
			{
				return waittime;
			}

			/**
			 * Resets all statistics and sets them to 0.
			 */
			void reset()
			{
				polygons = 0;
				batches = 0;
				fps = 0.0f;
			}
			/**
			 * Signals the class that the frame has started. This is called
			 * before waiting for the main thread.
			 */
			void setFrameBegin(core::Time time)
			{
				framebegin = time;
			}
			/**
			 * Signals the class that rendering has started. This is called
			 * after waiting for the main thread.
			 */
			void setRenderBegin(core::Time time)
			{
				renderbegin = time;
				waittime = renderbegin - framebegin;
			}
			/**
			 * Signals the class that rendering has finished. This also computes
			 * the different statistics like times or fps.
			 */
			void setFrameEnd(core::Time time)
			{
				frameend = time;
				rendertime = frameend - renderbegin;
				frametime = frameend - framebegin;
				// Compute frame per second
				fps = 1000000000.0f / frametime.getNanoseconds();
			}
			/**
			 * Signals the class that a certain number of batches has been
			 * rendered. This is called by VideoDriver::draw().
			 */
			void increaseBatchCount(unsigned int batches)
			{
				this->batches += batches;
			}
			/**
			 * Signals the class that a certain number of polygons has been
			 * rendered. This is called by VideoDriver::draw().
			 */
			void increasePolygonCount(unsigned int polygons)
			{
				this->polygons += polygons;
			}

			RenderStats &operator=(const RenderStats &other)
			{
				polygons = other.polygons;
				batches = other.batches;
				fps = other.fps;
				frametime = other.frametime;
				rendertime = other.rendertime;
				waittime = other.waittime;
				framebegin = other.framebegin;
				renderbegin = other.renderbegin;
				frameend = other.frameend;
				return *this;
			}
		private:
			unsigned int polygons;
			unsigned int batches;
			float fps;
			core::Duration frametime;
			core::Duration rendertime;
			core::Duration waittime;

			core::Time framebegin;
			core::Time renderbegin;
			core::Time frameend;
	};
}
}

#endif
