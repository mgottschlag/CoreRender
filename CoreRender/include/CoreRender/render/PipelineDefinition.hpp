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

#ifndef _CORERENDER_RENDER_PIPELINEDEFINITION_HPP_INCLUDED_
#define _CORERENDER_RENDER_PIPELINEDEFINITION_HPP_INCLUDED_

#include "../res/Resource.hpp"
#include "../core/StringTable.hpp"
#include "Pipeline.hpp"

#include <map>

class TiXmlElement;

namespace cr
{
namespace render
{
	struct CommandDefinition
	{
		PipelineCommandType::List type;
		std::vector<std::string> params;
	};

	struct StageDefinition
	{
		~StageDefinition()
		{
			for (unsigned int i = 0; i < commands.size(); i++)
				delete commands[i];
		}
		std::string name;
		std::vector<CommandDefinition*> commands;
	};

	struct SequenceDefinition
	{
		~SequenceDefinition()
		{
			for (unsigned int i = 0; i < stages.size(); i++)
				delete stages[i];
		}
		std::string name;
		std::vector<StageDefinition*> stages;
	};

	class PipelineDefinition : public res::Resource
	{
		public:
			PipelineDefinition(res::ResourceManager *rmgr, const std::string &name);
			virtual ~PipelineDefinition();

			SequenceDefinition *addSequence(const std::string &name);
			int findSequence(const std::string &name);
			SequenceDefinition *getSequence(const std::string &name);
			SequenceDefinition *getSequence(unsigned int index);
			void removeSequence(unsigned int index);
			unsigned int getSequenceCount();

			void setDefaultSequence(const std::string &sequence);
			std::string getDefaultSequence();

			void addDeferredLightLoop(const std::string &sequence);
			unsigned int getDeferredLightLoopCount();
			std::string getDeferredLightLoop(unsigned int index);

			void addForwardLightLoop(const std::string &sequence);
			unsigned int getForwardLightLoopCount();
			std::string getForwardLightLoop(unsigned int index);

			struct TextureInfo
			{
				std::string name;
				float size;
				TextureFormat::List format;
			};
			void addTexture(const std::string &name,
			                TextureFormat::List format,
			                float size = 1.0f);
			TextureInfo *getTexture(const std::string &name);
			void removeTexture(const std::string &name);
			unsigned int getTextureCount();

			struct FrameBufferInfo
			{
				std::string name;
				float size;
				bool depthbuffer;
			};

			void addFrameBuffer(const std::string &name,
			                    float size = 1.0f,
			                    bool depthbuffer = false);
			FrameBufferInfo *getFrameBuffer(const std::string &name);
			void removeFrameBuffer(const std::string &name);
			unsigned int getFrameBufferCount();

			struct RenderTargetInfo
			{
				std::string name;
				std::string framebuffer;
				std::string depthbuffer;
				std::vector<std::string> colorbuffers;
			};

			RenderTargetInfo *addRenderTarget(const std::string &name);
			RenderTargetInfo *getRenderTarget(const std::string &name);
			void removeRenderTarget(const std::string &name);
			unsigned int getRenderTargetCount();

			Pipeline::Ptr createPipeline();

			virtual bool load();

			const char *getType()
			{
				return "PipelineDefinition";
			}

			typedef core::SharedPointer<PipelineDefinition> Ptr;
		private:
			bool loadSequence(TiXmlElement *xml);
			void loadStage(SequenceDefinition *sequence, TiXmlElement *xml);

			std::vector<SequenceDefinition*> sequences;

			std::string defaultsequence;

			std::vector<std::string> forwardlightloops;
			std::vector<std::string> deferredlightloops;

			std::map<std::string, TextureInfo*> textures;
			std::map<std::string, FrameBufferInfo*> framebuffers;
			std::map<std::string, RenderTargetInfo*> rendertargets;
	};
}
}

#endif
