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

#ifndef _CORERENDER_SCENE_ANIMATION_HPP_INCLUDED_
#define _CORERENDER_SCENE_ANIMATION_HPP_INCLUDED_

#include "../res/Resource.hpp"

#include <GameMath.hpp>

namespace cr
{
namespace scene
{
	class Animation : public res::Resource
	{
		public:
			Animation(cr::res::ResourceManager* rmgr, const std::string& name);
			virtual ~Animation();

			/**
			 * Single animation sample which defines the position of one joint
			 * at a certain time.
			 */
			struct Sample
			{
				/**
				 * Rotation of the node.
				 */
				math::Quaternion rotation;
				/**
				 * Position of the node.
				 */
				math::Vec3f position;
				/**
				 * Scale of the node.
				 */
				math::Vec3f scale;
			};
			/**
			 * Animation frame which defines the position of all joints at a
			 * certain time.
			 */
			struct Frame
			{
				/**
				 * Animation samples for the different joints.
				 */
				std::vector<Sample> samples;
			};

			/**
			 * Sets the speed of the animation (in frames per second).
			 * @param fps Frames per second.
			 */
			void setFramesPerSecond(float fps)
			{
				this->fps = fps;
			}
			/**
			 * Returns the speed of the animation.
			 * @return Frames per second.
			 */
			float getFramesPerSecond()
			{
				return fps;
			}

			/**
			 * Sets the frame count of the animation.
			 * @param framecount New frame count.
			 */
			void setFrameCount(unsigned int framecount);
			/**
			 * Returns the frame count of the animations.
			 * @return Frame count.
			 */
			unsigned int getFrameCount()
			{
				return frames.size();
			}
			/**
			 * Returns the frame at a certain index.
			 * @param index Index of the frame.
			 * @return Frame at the index.
			 */
			Frame *getFrame(unsigned int index)
			{
				if (index >= frames.size())
					return 0;
				return frames[index];
			}
			/**
			 * Returns the frame at a certain time, without interpolating
			 * between frames.
			 * @param time Time of the frame.
			 * @return Nearest frame.
			 */
			Frame *getFrame(float time);
			/**
			 * Computes the frame at a certain time, interpolating between
			 * several frames if necessary.
			 * @param time Time of the frame.
			 * @param frame Resulting frame.
			 */
			void computeFrame(float time, Frame &result);

			/**
			 * Adds a channel to the animation.
			 * @param node Name of the node affected by the channel.
			 * @return Index of the new channel.
			 */
			unsigned int addChannel(const std::string &node);
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
			 * @return Index of the channel with the name or -1 if no channel
			 * was found.
			 */
			int getChannel(const std::string &node);
			/**
			 * Returns the name of the node of a channel.
			 * @param index Index of the channel.
			 * @return Name of the node affected by the channel.
			 */
			std::string getChannelNode(unsigned int index);
			/**
			 * Returns the number of channels in this animation.
			 * @return Number of channels.
			 */
			unsigned int getChannelCount();

			/**
			 * Returns a counter which is increased every time structural
			 * changes are made to the animation. This is used to optimize
			 * AnimationBinding::update() not to update if nothing changed.
			 * @return Change counter.
			 */
			unsigned int getChangeCounter()
			{
				return changecounter;
			}

			virtual bool load();

			virtual const char *getType()
			{
				return "Animation";
			}

			typedef core::SharedPointer<Animation> Ptr;
		private:
			std::vector<Frame*> frames;
			std::vector<std::string> channels;
			float fps;

			unsigned int changecounter;
	};
}
}

#endif
