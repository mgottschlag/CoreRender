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

#include "RenderPass.hpp"
#include "RenderJob.hpp"

namespace cr
{
namespace render
{
	class RenderJob;
	class Renderable;
	struct PipelineInfo;

	/**
	 * Class containing the definition about how to render geometry. The
	 * pipeline contains multiple render passes, for rendering one then calls
	 * Pipeline::submit() with the data to render. The pipeline then prepares
	 * render batches from the data passed to submit() and passes it to the
	 * renderer when Pipeline::prepare() is called which then draws the render
	 * passes in the order they were added to the pipeline.
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
			 * Adds a new render pass to the pipeline. The render pass is added
			 * after all existing passes.
			 * @param pass New render pass.
			 * @note Not thread-safe.
			 */
			void addPass(RenderPass::Ptr pass);
			/**
			 * Removes a render pass from the pipeline.
			 * @param pass Render pass to be removed.
			 */
			void removePass(RenderPass::Ptr pass);
			/**
			 * Removes a render pass from the pipeline.
			 * @param index Index of the pass to be removed.
			 */
			void removePass(unsigned int index);
			/**
			 * Returns the render pass with the given index.
			 * @param index Index of the render pass.
			 * @return Pass with the given index.
			 */
			RenderPass::Ptr getPass(unsigned int index);
			/**
			 * Returns the number of render passes attached to the pipeline.
			 */
			unsigned int getPassCount();

			/**
			 * Submits a renderable object to the pipeline and prepares batches
			 * for rendering.
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
			 * Begins a new frame. Called by Renderer, do not call this
			 * manually.
			 */
			void beginFrame();
			/**
			 * Prepares the pipeline data to be passed to the render thread and
			 * places it in info.
			 * @param info Pipeline render info to be passed to the render
			 * thread.
			 */
			void prepare(PipelineInfo *info);

			typedef core::SharedPointer<Pipeline> Ptr;
		private:
			std::vector<RenderPass::Ptr> passes;
	};
}
}

#endif
