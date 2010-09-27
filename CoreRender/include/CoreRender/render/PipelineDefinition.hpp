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
	/**
	 * Struct which holds the information necessary to construct a
	 * PipelineCommand in PipelineDefinition::createPipeline().
	 */
	struct CommandDefinition
	{
		PipelineCommandType::List type;
		std::vector<std::string> params;
	};

	/**
	 * Struct which holds the template for a single PipelineStage.
	 */
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

	/**
	 * Struct which holds the template for a single PipelineSequence.
	 */
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

	/**
	 * Resource class which holds the information needed to construct pipelines.
	 * This class can be used to load pipelines from files.
	 */
	class PipelineDefinition : public res::Resource
	{
		public:
			/**
			 * Constructor.
			 * @param rmgr Resource manager for this resource.
			 * @param name Name of this resource.
			 */
			PipelineDefinition(res::ResourceManager *rmgr, const std::string &name);
			/**
			 * Destructor.
			 */
			virtual ~PipelineDefinition();

			/**
			 * Adds a new sequence to the pipeline definition.
			 * @param name Name of the new sequence.
			 * @return New empty sequence.
			 */
			SequenceDefinition *addSequence(const std::string &name);
			/**
			 * Finds the index of the sequence with the given name.
			 * @param name Name of the sequence.
			 * @return Index of the sequence or -1 if it was not found.
			 */
			int findSequence(const std::string &name);
			/**
			 * Returns the sequence with the given name.
			 * @param name Name of the sequence.
			 * @return Sequence with the name.
			 */
			SequenceDefinition *getSequence(const std::string &name);
			/**
			 * Returns the sequence at a specific index.
			 * @param index Index of the sequence.
			 * @return Sequence at the index.
			 */
			SequenceDefinition *getSequence(unsigned int index);
			/**
			 * Removes the sequence at a specific index.
			 * @param index Index of the sequence to be removed.
			 */
			void removeSequence(unsigned int index);
			/**
			 * Returns the number of sequences in the pipeline definition.
			 */
			unsigned int getSequenceCount();

			/**
			 * Sets the name of the default sequence. Note that the sequence
			 * should have been added via addSequence() before. If "" is passed
			 * as a name, no default sequence is selected.
			 * @param sequence Name of the default sequence.
			 */
			void setDefaultSequence(const std::string &sequence);
			/**
			 * Returns the name of the default sequence.
			 * @return Name of the default sequence or "" if no default sequence
			 * was selected.
			 */
			std::string getDefaultSequence();

			/**
			 * Registers a sequence as a deferred light loop.
			 * @param sequence Name of the sequence.
			 */
			void addDeferredLightLoop(const std::string &sequence);
			/**
			 * Returns the number of deferred light loops.
			 * @return Number of registered sequences.
			 */
			unsigned int getDeferredLightLoopCount();
			/**
			 * Gets the name of a deferred light loop at a certain index.
			 * @param index Index of the light loop.
			 * @return Name of the sequence.
			 */
			std::string getDeferredLightLoop(unsigned int index);

			/**
			 * Registers a sequence as a forward light loop.
			 * @param sequence Name of the sequence.
			 */
			void addForwardLightLoop(const std::string &sequence);
			/**
			 * Returns the number of forward light loops.
			 * @return Number of registered sequences.
			 */
			unsigned int getForwardLightLoopCount();
			/**
			 * Gets the name of a forward light loop at a certain index.
			 * @param index Index of the light loop.
			 * @return Name of the sequence.
			 */
			std::string getForwardLightLoop(unsigned int index);

			/**
			 * Struct which contains information about a texture used for a
			 * render target.
			 */
			struct TextureInfo
			{
				std::string name;
				float size;
				TextureFormat::List format;
			};
			/**
			 * Adds a texture for use with a render target. The textures added
			 * like this are created for every pipeline created via
			 * createPipeline(), they are not shared among pipelines.
			 * @param name Local name of the texture.
			 * @param format Format of the texture.
			 * @param size Size of the texture. 1.0 means the size of the
			 * screen, 0.5 the half size of the screen.
			 */
			void addTexture(const std::string &name,
			                TextureFormat::List format,
			                float size = 1.0f);
			/**
			 * Returns the texture info with the given name.
			 * @param name Internal name of the texture.
			 * @return Texture with the name.
			 */
			TextureInfo *getTexture(const std::string &name);
			/**
			 * Removes the texture info with the given name.
			 * @param name Internal name of the texture.
			 */
			void removeTexture(const std::string &name);
			/**
			 * Returns the number of textures which are created at every call
			 * to createPipeline().
			 * @return Number of textures added to this pipeline definition.
			 */
			unsigned int getTextureCount();

			/**
			 * Struct which contains information about a frame buffer used for
			 * the pipelines.
			 */
			struct FrameBufferInfo
			{
				std::string name;
				float size;
				bool depthbuffer;
			};

			/**
			 * Adds a frame buffer for use with a render target. The frame
			 * buffers added like this are created for every pipeline created
			 * via createPipeline(), they are not shared among pipelines.
			 * @param name Local name of the frame buffer.
			 * @param size Size of the texture. 1.0 means the size of the
			 * screen, 0.5 the half size of the screen.
			 * @param depthbuffer True if an depth buffer should be created for
			 * the frame buffer.
			 */
			void addFrameBuffer(const std::string &name,
			                    float size = 1.0f,
			                    bool depthbuffer = false);
			/**
			 * Returns the frame buffer info with the given name.
			 * @param name Internal name of the frame buffer.
			 * @return Frame buffer with the name.
			 */
			FrameBufferInfo *getFrameBuffer(const std::string &name);
			/**
			 * Removes the frame buffer info with the given name.
			 * @param name Internal name of the frame buffer.
			 */
			void removeFrameBuffer(const std::string &name);
			/**
			 * Returns the number of frame buffers which are created at every
			 * call to createPipeline().
			 * @return Number of frame buffers previously added to this pipeline
			 * definition.
			 */
			unsigned int getFrameBufferCount();

			/**
			 * Struct which contains information about a render target used for
			 * the pipelines.
			 */
			struct RenderTargetInfo
			{
				std::string name;
				std::string framebuffer;
				std::string depthbuffer;
				std::vector<std::string> colorbuffers;
			};

			/**
			 * Adds a render target to the pipeline definition.
			 * @param name Local name of the new render target.
			 * @return New render target.
			 */
			RenderTargetInfo *addRenderTarget(const std::string &name);
			/**
			 * Returns the render target with a certain name.
			 * @param name Local name of the render target.
			 * @return Render target with the name.
			 */
			RenderTargetInfo *getRenderTarget(const std::string &name);
			/**
			 * Removes the render target with a certain name.
			 * @param name Local name of the render target.
			 */
			void removeRenderTarget(const std::string &name);
			/**
			 * Returns the number of render targets created for the pipelines.
			 * @return Number of render targets.
			 */
			unsigned int getRenderTargetCount();

			/**
			 * Creates a new pipeline instance which then can be used for
			 * rendering. The render targets in the pipeline are created for
			 * a certain screen size which has to be specified here.
			 * @param screenwidth Width of the final render target.
			 * @param screenheight Height of the final render target.
			 * @return New pipeline instance.
			 */
			Pipeline::Ptr createPipeline(unsigned int screenwidth = 512,
			                             unsigned int screenheight = 512);

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
