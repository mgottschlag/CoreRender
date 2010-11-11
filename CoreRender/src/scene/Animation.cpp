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

#include "CoreRender/scene/Animation.hpp"
#include "CoreRender/scene/AnimationFile.hpp"
#include "CoreRender/res/ResourceManager.hpp"

namespace cr
{
namespace scene
{
	Animation::Animation(cr::res::ResourceManager *rmgr,
	                     const std::string &name)
		: Resource(rmgr, name), fps(50.0f), changecounter(0)
	{
	}
	Animation::~Animation()
	{
		for (unsigned int i = 0; i < frames.size(); i++)
			delete frames[i];
	}

	void Animation::setFrameCount(unsigned int framecount)
	{
		unsigned int oldframecount = frames.size();
		if (framecount == oldframecount)
			return;
		if (framecount > oldframecount)
		{
			frames.resize(framecount);
			for (unsigned int i = oldframecount; i < framecount; i++)
			{
				Frame *newframe;
				// If possible, we copy the data from the oldest available frame
				if (oldframecount != 0)
					newframe = new Frame(*frames[oldframecount - 1]);
				else
					newframe = new Frame;
				frames[i] = newframe;
			}
		}
		else
		{
			for (unsigned int i = framecount; i < oldframecount; i++)
			{
				delete frames[i];
			}
			frames.resize(framecount);
		}
		changecounter++;
	}

	Animation::Frame *Animation::getFrame(float time)
	{
		if (frames.size() == 0)
			return 0;
		// TODO: Fast conversion
		int frameindex = time * fps;
		if (frameindex < 0)
			frameindex = 0;
		if ((unsigned int)frameindex >= frames.size())
			frameindex = frames.size() - 1;
		return frames[frameindex];
	}
	void Animation::computeFrame(float time, Animation::Frame &result)
	{
		if (frames.size() == 0)
			return;
		if (time < 0.0f)
		{
			result = *frames[0];
			return;
		}
		// TODO: Fast conversion
		unsigned int frameindex = time * fps;
		if (frameindex >= frames.size() - 1)
		{
			result = *frames[frames.size() - 1];
			return;
		}
		float d = time * fps - frameindex;
		if (d == 0)
		{
			result = *frames[frameindex];
			return;
		}
		// Interpolate between two frames;
		Frame &frame1 = *frames[frameindex];
		Frame &frame2 = *frames[frameindex + 1];
		unsigned int samplecount = frame1.samples.size();
		result.samples.resize(samplecount);
		for (unsigned int i = 0; i < samplecount; i++)
		{
			Sample &sample = result.samples[i];
			Sample &src1 = frame1.samples[i];
			Sample &src2 = frame2.samples[i];
			sample.position.interpolate(src1.position, src2.position, d);
			sample.scale.interpolate(src1.scale, src2.scale, d);
			sample.rotation.interpolate(src1.rotation, src2.rotation, d);
		}
	}

	unsigned int Animation::addChannel(const std::string &node)
	{
		unsigned int oldchannelcount = channels.size();
		channels.push_back(node);
		for (unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i]->samples.push_back(Sample());
		}
		changecounter++;
		return oldchannelcount;
	}
	void Animation::removeChannel(const std::string &node)
	{
		for (unsigned int i = 0; i < channels.size(); i++)
		{
			if (channels[i] == node)
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
		for (unsigned int i = 0; i < frames.size(); i++)
		{
			frames[i]->samples.erase(frames[i]->samples.begin() + index);
		}
		channels.erase(channels.begin() + index);
		changecounter++;
	}
	int Animation::getChannel(const std::string &node)
	{
		for (unsigned int i = 0; i < channels.size(); i++)
		{
			if (channels[i] == node)
				return i;
		}
		return -1;
	}
	std::string Animation::getChannelNode(unsigned int index)
	{
		if (index >= channels.size())
			return "";
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
		if (header.framespersecond != 0.0f)
			setFramesPerSecond(header.framespersecond);
		else
			setFramesPerSecond(50);
		// Allocate data
		frames.resize(header.framecount);
		for (unsigned int i = 0; i < header.framecount; i++)
		{
			frames[i] = new Frame();
			frames[i]->samples.resize(header.channelcount);
		}
		channels.resize(header.channelcount);
		// Load channels
		changecounter++;
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
			channels[i] = channelhdr.name;
			// TODO: Constant channels?
			//bool isconstant = channelhdr.constant;
				for (unsigned int j = 0; j < framecount; j++)
			{
				AnimationFile::Frame &src = framedata[j];
				Sample &sample = frames[j]->samples[i];
				sample.rotation.x = src.rotation[0];
				sample.rotation.y = src.rotation[1];
				sample.rotation.z = src.rotation[2];
				sample.rotation.w = src.rotation[3];
				sample.position.x = src.position[0];
				sample.position.y = src.position[1];
				sample.position.z = src.position[2];
				sample.scale.x = src.scale[0];
				sample.scale.y = src.scale[1];
				sample.scale.z = src.scale[2];
			}
		}
		finishLoading(true);
		return true;
	}
}
}
