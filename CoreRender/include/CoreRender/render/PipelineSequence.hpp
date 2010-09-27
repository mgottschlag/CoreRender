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

#ifndef _CORERENDER_RENDER_PIPELINESEQUENCE_HPP_INCLUDED_
#define _CORERENDER_RENDER_PIPELINESEQUENCE_HPP_INCLUDED_

#include "DefaultUniform.hpp"
#include "Texture.hpp"

#include <string>
#include <vector>
#include <map>

namespace cr
{
namespace render
{
	class PipelineStage;
	class RenderJob;
	class Renderable;
	class Renderer;
	struct PipelineInfo;
	struct PipelineSequenceInfo;
	class BatchListCommand;

	/**
	 * Current sequence state which is used to keep track of bound textures
	 * for PipelineCommand::apply().
	 */
	struct SequenceState
	{
		std::map<std::string, Texture::Ptr> textures;
	};

	/**
	 * Class which defines how the scene is rendered for a single camera.
	 */
	class PipelineSequence
	{
		public:
			/**
			 * Constructor.
			 * @param name Name of the sequence.
			 */
			PipelineSequence(const std::string &name);
			/**
			 * Destructor.
			 */
			~PipelineSequence();

			/**
			 * Adds a new stage to the pipeline.
			 * @param name Name of the new stage.
			 * @return New stage.
			 */
			PipelineStage *addStage(const std::string &name);
			/**
			 * Returns the index of the stage with a certain name.
			 * @param name Name of the stage.
			 * @return Index of the stage or -1 if no stage was found.
			 */
			int findStage(const std::string &name);
			/**
			 * Returns the stage with a certain name.
			 * @param name Name of the stage.
			 * @return Stage with the name or 0 if no stage was found.
			 */
			PipelineStage *getStage(const std::string &name);
			/**
			 * Returns the stage at a certain index.
			 * @param index Index of the stage.
			 * @return Stage at the index.
			 */
			PipelineStage *getStage(unsigned int index);
			/**
			 * Removes the stage at a certain index.
			 * @param index Index of the stage.
			 */
			void removeStage(unsigned int index);
			/**
			 * Returns the number of stages in this sequence.
			 * @return Number of stages.
			 */
			unsigned int getStageCount();

			/**
			 * Returns the default uniform values which are set for this
			 * sequence. This is mostly used to set camera settings which are
			 * valid for all jobs rendered with this sequence. This function
			 * returns a reference which can be used to add or modify uniform
			 * values.
			 * @return Reference to the default uniform values.
			 */
			std::vector<DefaultUniform> &getDefaultUniforms()
			{
				return uniforms;
			}

			/**
			 * Submits a renderable object to the render sequence and prepares
			 * batches for rendering.
			 * @param renderable Renderable object to be rendered.
			 */
			void submit(Renderable *renderable);
			/**
			 * Submits a single render job to be rendered and prepares batches
			 * for rendering.
			 * @param job Render job to be rendered.
			 */
			void submit(RenderJob *job);

			/**
			 * Prepares all commands in the stages for rendering. This allocates
			 * the necessary memory needed to move the information to the render
			 * thread and prepares the commands if possible.
			 * @param renderer Renderer which is used for rendering.
			 */
			void beginFrame(Renderer *renderer);
			/**
			 * Prepares the batch lists on all stages for rendering and puts the
			 * data into info for it to be sent to the render thread.
			 * @param info Pipeline info which is fille with the render data.
			 */
			void prepare(PipelineInfo *info);

			/**
			 * Returns the name of the pipeline.
			 * @return Name of the pipeline.
			 */
			std::string getName();

			/**
			 * Returns the current state of the sequence. This is used by
			 * commands within beginFrame() to keep track of the textures which
			 * are bound for the sequence. Do not use this directly as this has
			 * no meaning for anything else.
			 * @return Current sequence state.
			 */
			SequenceState &getState()
			{
				return state;
			}
		private:
			std::string name;

			std::vector<PipelineStage*> stages;
			std::vector<BatchListCommand*> batchlists;

			PipelineSequenceInfo *info;

			Renderer *renderer;

			std::vector<DefaultUniform> uniforms;

			SequenceState state;
	};
}
}

#endif
