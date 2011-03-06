/*
Copyright (C) 2011, Mathias Gottschlag

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

#include "../core/Time.hpp"

namespace cr
{
class GraphicsEngine;
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
				: polygons(0), batches(0), fps(0.0f), averagefps(0),
				frametime(core::Duration::Seconds(0)),
				averageframetime(core::Duration::Seconds(0)),
				uploadtime(core::Duration::Seconds(0)),
				averageuploadtime(core::Duration::Seconds(0)),
				rendertime(core::Duration::Seconds(0)),
				averagerendertime(core::Duration::Seconds(0)),
				composetime(core::Duration::Seconds(0)),
				averagecomposetime(core::Duration::Seconds(0))
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
				averagefps = other.averagefps;
				frametime = other.frametime;
				averageframetime = other.averageframetime;
				rendertime = other.rendertime;
				averagerendertime = other.averagerendertime;
				composetime = other.composetime;
				averagecomposetime = other.averagecomposetime;
				uploadtime = other.uploadtime;
				averageuploadtime = other.averageuploadtime;
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
			 * is only measured based on the time between the last and this
			 * call to GraphicsEngine::render(), so this number will vary
			 * wildly. Use getAverageFPS() for more stable and usable numbers.
			 */
			float getFPS() const
			{
				return fps;
			}
			/**
			 * Returns the average frames per second of all frames of about the
			 * last second.
			 */
			float getAverageFPS() const
			{
				return averagefps;
			}
			/**
			 * Returns the time needed to for the frame. This includes both the
			 * time needed for rendering (getRenderTime()) and the time needed
			 * to compose the frame (getComposeTime()).
			 */
			core::Duration getFrameTime() const
			{
				return frametime;
			}
			/**
			 * Returns the average frame time of all frames of about the last
			 * second.
			 */
			core::Duration getAverageFrameTime() const
			{
				return averageframetime;
			}
			/**
			 * Returns the time needed foruploading of resources.
			 */
			core::Duration getUploadTime() const
			{
				return uploadtime;
			}
			/**
			 * Returns the average upload time of all frames of about the last
			 * second.
			 */
			core::Duration getAverageUploadTime() const
			{
				return averageuploadtime;
			}
			/**
			 * Returns the time needed for only rendering
			 * (GraphicsEngine::render()).
			 */
			core::Duration getRenderTime() const
			{
				return rendertime;
			}
			/**
			 * Returns the average render time of all frames of about the last
			 * second.
			 */
			core::Duration getAverageRenderTime() const
			{
				return averagerendertime;
			}
			/**
			 * Returns the time which was needed to compose the frame (between
			 * GraphicsEngine::beginFrame() and GraphicsEngine::endFrame()).
			 */
			core::Duration getComposeTime() const
			{
				return composetime;
			}
			/**
			 * Returns the average compose time of all frames of about the last
			 * second.
			 */
			core::Duration getAverageComposeTime() const
			{
				return averagecomposetime;
			}

			/**
			 * Resets all statistics and sets them to 0.
			 */
			void reset()
			{
				polygons = 0;
				batches = 0;
				fps = 0.0f;
				averagefps = 0.0f;
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
				averagefps = other.averagefps;
				frametime = other.frametime;
				averageframetime = other.averageframetime;
				rendertime = other.rendertime;
				averagerendertime = other.averagerendertime;
				composetime = other.composetime;
				averagecomposetime = other.averagecomposetime;
				uploadtime = other.uploadtime;
				averageuploadtime = other.averageuploadtime;
				return *this;
			}
		private:
			friend class cr::GraphicsEngine;

			unsigned int polygons;
			unsigned int batches;
			float fps;
			float averagefps;
			core::Duration frametime;
			core::Duration averageframetime;
			core::Duration uploadtime;
			core::Duration averageuploadtime;
			core::Duration rendertime;
			core::Duration averagerendertime;
			core::Duration composetime;
			core::Duration averagecomposetime;
	};
}
}

#endif
