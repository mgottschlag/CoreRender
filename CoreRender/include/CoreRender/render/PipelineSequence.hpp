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

#include <string>
#include <vector>

namespace cr
{
namespace render
{

class BatchListCommand;

	class PipelineStage;
	class RenderJob;
	class Renderable;
	class Renderer;
	struct PipelineInfo;
	struct PipelineSequenceInfo;

	class PipelineSequence
	{
		public:
			PipelineSequence(const std::string &name);
			~PipelineSequence();

			PipelineStage *addStage(const std::string &name);
			int findStage(const std::string &name);
			PipelineStage *getStage(const std::string &name);
			PipelineStage *getStage(unsigned int index);
			void removeStage(unsigned int index);
			unsigned int getStageCount();

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

			void beginFrame(Renderer *renderer);
			void prepare(PipelineInfo *info);

			std::string getName();
		private:
			std::string name;

			std::vector<PipelineStage*> stages;
			std::vector<BatchListCommand*> batchlists;

			PipelineSequenceInfo *info;

			Renderer *renderer;

			std::vector<DefaultUniform> uniforms;
	};
}
}

#endif
