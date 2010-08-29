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

#ifndef _CORERENDER_RENDER_ANIMATION_HPP_INCLUDED_
#define _CORERENDER_RENDER_ANIMATION_HPP_INCLUDED_

#include "../res/Resource.hpp"
#include "../math/Matrix4.hpp"
#include "../math/Quaternion.hpp"

namespace cr
{
namespace render
{
	class VideoDriver;
	class Renderer;

	/**
	 * Resource which holds several animation channels which can be used to
	 * animate a model.
	 *
	 * This class should be created via ResourceManager::getOrCreate().
	 */
	class Animation : public res::Resource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			Animation(res::ResourceManager *rmgr,
			          const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~Animation();

			/**
			 * Single animation frame. The content of this struct defines the
			 * relative transformation of a node at a certain point of time.
			 * @note This transformation is not additive, but rather replaces
			 * the transformation matrix of the node.
			 */
			struct Frame
			{
				/**
				 * Rotation of the node.
				 */
				math::Quaternion rotation;
				/**
				 * Position of the node.
				 */
				math::Vector3F position;
				/**
				 * Scale of the node.
				 */
				math::Vector3F scale;
			};
			/**
			 * Single animation channel for a single model node.
			 */
			struct Channel
			{
				/**
				 * Name of the model node.
				 */
				std::string node;
				/**
				 * If set to true, only one animation frame is provided which
				 * stays constant for the whole animation time.
				 */
				bool constant;
				/**
				 * List with the frames for this channel. This will have
				 * Animation::getFrameCount() entries (or 1 if the channel is
				 * set to be constant).
				 */
				std::vector<Frame> frames;
			};

			/**
			 * Sets the frame count of the animation.
			 * @param framecount New frame count.
			 */
			void setFrameCount(unsigned int framecount);
			/**
			 * Returns the frame count of the animations.
			 * @return Frame count.
			 */
			unsigned int getFrameCount();

			/**
			 * Sets the speed of the animation (in frames per second).
			 * @param fps Frames per second.
			 */
			void setFramesPerSecond(float fps);
			/**
			 * Returns the speed of the animation.
			 * @return Frames per second.
			 */
			float getFramesPerSecond();

			/**
			 * Adds a channel to the animation.
			 * @param node Name of the node affected by the channel.
			 */
			Channel *addChannel(const std::string &node);
			/**
			 * Removes a channel affecting a certain node.
			 * @param node Name of the channel to be removed.
			 */
			void removeChannel(const std::string &node);
			/**
			 * Removes a channel at a certain index.
			 * @param index Index of the channel to be removed.
			 */
			void removeChannel(unsigned int index);
			/**
			 * Returns the channel with a certain name.
			 * @return Channel with the specified name or 0 if no channel was
			 * found.
			 */
			Channel *getChannel(const std::string &node);
			/**
			 * Returns the channel at a certain index.
			 * @return Channel at the index, or 0 if index is too large.
			 */
			Channel *getChannel(unsigned int index);
			/**
			 * Returns the number of channels in this animation.
			 * @return Number of channels.
			 */
			unsigned int getChannelCount();
			/**
			 * Returns the frame for a certain channel at the specified time.
			 * This automatically interpolates between two frames if necessary.
			 * @param channel Channel to take the frames from.
			 * @param time Animation time in seconds.
			 * @return Resulting frame for this time.
			 * @todo Pass frame as reference.
			 * @note This function clips the time into the range of the
			 * animation and does no automatic looping of the animation.
			 */
			Frame getFrame(Channel *channel, float time);

			virtual bool load();

			virtual const char *getType()
			{
				return "Animation";
			}

			typedef core::SharedPointer<Animation> Ptr;
		private:
			unsigned int framecount;
			float fps;
			std::vector<Channel*> channels;
	};
}
}

#endif
