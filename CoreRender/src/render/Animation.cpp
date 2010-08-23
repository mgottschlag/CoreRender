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

#include "CoreRender/render/Animation.hpp"
#include "CoreRender/core/FileSystem.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/AnimationFile.hpp"

namespace cr
{
namespace render
{
	Animation::Animation(res::ResourceManager *rmgr,
	                     const std::string &name)
		: Resource(rmgr, name), framecount(0), fps(20)
	{
	}
	Animation::~Animation()
	{
		for (unsigned int i = 0; i < channels.size(); i++)
			delete channels[i];
	}

	void Animation::setFrameCount(unsigned int framecount)
	{
		this->framecount = framecount;
		// TODO: Update channels
	}
	unsigned int Animation::getFrameCount()
	{
		return framecount;
	}

	void Animation::setFramesPerSecond(float fps)
	{
		this->fps = fps;
	}
	float Animation::getFramesPerSecond()
	{
		return fps;
	}

	Animation::Channel *Animation::addChannel(const std::string &node)
	{
		Animation::Channel *channel = new Animation::Channel;
		channel->node = node;
		channel->constant = false;
		channels.push_back(channel);
		return channel;
	}
	void Animation::removeChannel(const std::string &node)
	{
		for (unsigned int i = 0; i < channels.size(); i++)
		{
			if (channels[i]->node == node)
			{
				removeChannel(i);
				return;
			}
		}
	}
	void Animation::removeChannel(unsigned int index)
	{
		if (index >= channels.size())
			return;
		delete channels[index];
		channels[index] = channels[channels.size() - 1];
		channels.pop_back();
	}
	Animation::Channel *Animation::getChannel(const std::string &node)
	{
		for (unsigned int i = 0; i < channels.size(); i++)
		{
			if (channels[i]->node == node)
				return channels[i];
		}
		return 0;
	}
	Animation::Channel *Animation::getChannel(unsigned int index)
	{
		if (index >= channels.size())
			return 0;
		return channels[index];
	}
	unsigned int Animation::getChannelCount()
	{
		return channels.size();
	}

	bool Animation::load()
	{
		std::string path = getPath();
		std::string directory = core::FileSystem::getDirectory(path);
		// Open file
		core::FileSystem::Ptr fs = getManager()->getFileSystem();
		core::File::Ptr file = fs->open(path, core::FileAccess::Read);
		if (!file)
		{
			getManager()->getLog()->error("Could not open file \"%s\".",
			                               path.c_str());
			finishLoading(false);
			return false;
		}
		// Load file header
		// TODO: Adapt all this to big-endian
		AnimationFile::Header header;
		if (file->read(sizeof(header), &header) != sizeof(header))
		{
			getManager()->getLog()->error("%s: Could not read animation header.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		if (header.tag != AnimationFile::tag
		 || header.version != AnimationFile::version)
		{
			getManager()->getLog()->error("%s: Invalid animation file.",
			                              getName().c_str());
			finishLoading(false);
			return false;
		}
		// Load channels
		for (unsigned int i = 0; i < header.channelcount; i++)
		{
			// Read animation channel header
			AnimationFile::Channel channelhdr;
			if (file->read(sizeof(channelhdr), &channelhdr) != sizeof(channelhdr))
			{
				getManager()->getLog()->error("%s: Could not read animation channel.",
				                              getName().c_str());
				finishLoading(false);
				return false;
			}
			channelhdr.name[AnimationFile::maxnamesize - 1] = 0;
			// Read frames
			unsigned int framecount = header.framecount;
			if (channelhdr.constant)
				framecount = 1;
			std::vector<AnimationFile::Frame> framedata(framecount);
			int datasize = sizeof(AnimationFile::Frame) * framecount;
			if (file->read(datasize, &framedata[0]) != datasize)
			{
				getManager()->getLog()->error("%s: Could not read channel data.",
				                              getName().c_str());
				finishLoading(false);
				return false;
			}
			// Add channel
			Channel *channel = addChannel(channelhdr.name);
			channel->constant = channelhdr.constant;
			channel->frames.resize(framecount);
			for (unsigned int i = 0; i < framecount; i++)
			{
				AnimationFile::Frame &src = framedata[i];
				Frame &dest = channel->frames[i];
				dest.rotation.q[0] = src.rotation[0];
				dest.rotation.q[1] = src.rotation[1];
				dest.rotation.q[2] = src.rotation[2];
				dest.rotation.q[3] = src.rotation[3];
				dest.position.x = src.position[0];
				dest.position.y = src.position[1];
				dest.position.z = src.position[2];
				dest.scale.x = src.scale[0];
				dest.scale.y = src.scale[1];
				dest.scale.z = src.scale[2];
			}
		}
		finishLoading(true);
		return true;
	}
}
}
