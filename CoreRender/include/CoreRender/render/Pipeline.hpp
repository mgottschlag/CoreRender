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

			PipelineSequence *addSequence(const std::string &name);
			int findSequence(const std::string &name);
			PipelineSequence *getSequence(const std::string &name);
			PipelineSequence *getSequence(unsigned int index);
			void removeSequence(unsigned int index);
			unsigned int getSequenceCount();

			void setDefaultSequence(PipelineSequence *sequence);
			PipelineSequence *getDefaultSequence();

			void addDeferredLightLoop(PipelineSequence *sequence);
			unsigned int getDeferredLightLoopCount();
			PipelineSequence *getDeferredLightLoop(unsigned int index);
			
			void addForwardLightLoop(PipelineSequence *sequence);
			unsigned int getForwardLightLoopCount();
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
