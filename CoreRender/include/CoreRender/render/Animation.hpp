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

	class Animation : public res::Resource
	{
		public:
			Animation(res::ResourceManager *rmgr,
			          const std::string &name);
			virtual ~Animation();

			struct Frame
			{
				math::Quaternion rotation;
				math::Vector3F position;
				math::Vector3F scale;
			};
			struct Channel
			{
				std::string node;
				bool constant;
				std::vector<Frame> frames;
			};

			void setFrameCount(unsigned int framecount);
			unsigned int getFrameCount();

			void setFramesPerSecond(float fps);
			float getFramesPerSecond();

			Channel *addChannel(const std::string &node);
			void removeChannel(const std::string &node);
			void removeChannel(unsigned int index);
			Channel *getChannel(const std::string &node);
			Channel *getChannel(unsigned int index);
			unsigned int getChannelCount();
			/**
			 * @todo Pass frame as reference.
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
