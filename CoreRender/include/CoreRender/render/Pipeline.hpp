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

#ifndef _CORERENDER_RENDER_PIPELINE_HPP_INCLUDED_
#define _CORERENDER_RENDER_PIPELINE_HPP_INCLUDED_

#include "RenderJob.hpp"
#include "PipelineCommand.hpp"

namespace cr
{
namespace render
{
	struct PipelineInfo;
	class Renderer;
	class PipelineSequence;

	/**
	 * Class which defines the way a scene is rendered to the screen or to
	 * a texture. The pipeline contains several sequences which each correlate
	 * to a single camera in the screen and can hold a set of default uniforms
	 * like the projection matrix. Each sequence is then composed of multiple
	 * stages which hold the rendering commands.
	 */
	class Pipeline : public core::ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 */
			Pipeline();
			/**
			 * Destructor.
			 */
			virtual ~Pipeline();

			/**
			 * Adds a new sequence at the end of the pipeline.
			 * @param name Name of the sequence.
			 * @return New sequence.
			 */
			PipelineSequence *addSequence(const std::string &name);
			/**
			 * Returns the index of a sequence with a certain name.
			 * @param name Name of the sequence.
			 * @return Index of the sequence or -1 if no sequence was found.
			 */
			int findSequence(const std::string &name);
			/**
			 * Returns the sequence with a certain name.
			 * @param name Name of the sequence.
			 * @return Sequence with the name.
			 */
			PipelineSequence *getSequence(const std::string &name);
			/**
			 * Returns the sequence at a certain index.
			 * @param index Index of the sequence.
			 * @return Sequence at the index.
			 */
			PipelineSequence *getSequence(unsigned int index);
			/**
			 * Removes the sequence at a certain index.
			 * @param index Index of the sequence to remove.
			 */
			void removeSequence(unsigned int index);
			/**
			 * Returns the number of sequences attached to this pipeline.
			 * @return Number of sequences.
			 */
			unsigned int getSequenceCount();

			/**
			 * Sets the default sequence for this pipeline. This sequence is the
			 * starting point for all rendering, other sequences within the
			 * pipeline then have to be rendered via SequenceCommand.
			 * @param sequence New default sequence.
			 */
			void setDefaultSequence(PipelineSequence *sequence);
			/**
			 * Returns the default sequence of this pipeline.
			 * @return Default sequence.
			 */
			PipelineSequence *getDefaultSequence();

			/**
			 * Adds a sequence to the list of deferred light loops. Each
			 * sequence in this pass will just hold more sequences for all
			 * shadow map generation passes and one batch per light for deferred
			 * lighting.
			 * @param sequence Sequence to be added to the light loop list.
			 */
			void addDeferredLightLoop(PipelineSequence *sequence);
			/**
			 * Returns the number of deferred light loops in this pipeline.
			 * @return Number of light loops.
			 */
			unsigned int getDeferredLightLoopCount();
			/**
			 * Returns a single sequence which holds a deferred light loop.
			 * @param index Index of the light loop.
			 */
			PipelineSequence *getDeferredLightLoop(unsigned int index);
			
			/**
			 * Adds a sequence to the list of forward light loops. Each
			 * sequence in this pass will just hold more sequences for all
			 * shadow map generation passes and for drawing the scene again
			 * with additive light materials.
			 * @param sequence Sequence to be added to the light loop list.
			 */
			void addForwardLightLoop(PipelineSequence *sequence);
			/**
			 * Returns the number of forward light loops in this pipeline.
			 * @return Number of light loops.
			 */
			unsigned int getForwardLightLoopCount();
			/**
			 * Returns a single sequence which holds a forward light loop.
			 * @param index Index of the light loop.
			 */
			PipelineSequence *getForwardLightLoop(unsigned int index);

			/**
			 * Begins a new frame. Called by Renderer, do not call this
			 * manually.
			 * @param renderer Renderer which renders this frame.
			 */
			void beginFrame(Renderer *renderer);
			/**
			 * Prepares the pipeline data to be passed to the render thread and
			 * places it in info.
			 * @param info Pipeline render info to be passed to the render
			 * thread.
			 */
			void prepare(PipelineInfo *info);

			typedef core::SharedPointer<Pipeline> Ptr;
		private:
			std::vector<PipelineSequence*> sequences;

			PipelineSequence *defaultsequence;

			std::vector<PipelineSequence*> forwardlightloops;
			std::vector<PipelineSequence*> deferredlightloops;
	};
}
}

#endif
