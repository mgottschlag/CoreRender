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

#ifndef _CORERENDER_RENDER_RENDERPASS_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERPASS_HPP_INCLUDED_

#include "RenderTarget.hpp"
#include "RenderPassInfo.hpp"

namespace cr
{
namespace render
{
	class VideoDriver;

	struct RenderBatch;

	/**
	 * A render pass draws geometry with one single material context. A pipeline
	 * then can contain many of these to draw different contexts.
	 *
	 * If you submit geometry to a pipeline, the pipeline first checks whether
	 * ShaderText::hasContext() is true before it proceeds with passing the
	 * batch to the render pass via RenderPass::insert().
	 */
	class RenderPass : public core::ReferenceCounted
	{
		public:
			/**
			 * Constructor.
			 * @param context Name of the context to be rendered in this pass.
			 */
			RenderPass(const std::string &context);
			/**
			 * Destructor.
			 */
			virtual ~RenderPass();

			/**
			 * Sets the render target for this pass.
			 * @param target New render target.
			 */
			void setRenderTarget(RenderTarget::Ptr target);
			/**
			 * Returns the render target used for this pass.
			 * @return Render target or 0 if rendering to the screen.
			 */
			RenderTarget::Ptr getRenderTarget();

			/**
			 * Called by Pipeline::beginFrame(), do not call this manually.
			 */
			void beginFrame();
			/**
			 * Inserts a render patch into the queue for this render pass.
			 * Called by Pipeline::submit(), do not call this manually.
			 * @param batch Batch to be drawn.
			 */
			void insert(RenderBatch *batch);
			/**
			 * Prepares and then returns the batch list for this pass.
			 * @param info Target for the batch list.
			 */
			void prepare(RenderPassInfo *info);

			/**
			 * Returns the context name for shaders drawn in this pass.
			 * @return Context name.
			 */
			const std::string &getContext()
			{
				return context;
			}

			typedef core::SharedPointer<RenderPass> Ptr;
		private:
			tbb::spin_mutex insertmutex;
			std::vector<RenderBatch*> batches;

			// TODO: We need info about the target etc here
			RenderBatch **prepared;
			unsigned int preparedcount;

			RenderTarget::Ptr target;

			std::string context;
	};
}
}

#endif
