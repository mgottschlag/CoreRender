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

#ifndef _CORERENDER_RENDER_PIPELINECOMMANDTYPE_HPP_INCLUDED_
#define _CORERENDER_RENDER_PIPELINECOMMANDTYPE_HPP_INCLUDED_

#include "../core/Color.hpp"
#include "Texture.hpp"
#include "Material.hpp"

#include <vector>
#include "RenderTarget.hpp"

namespace cr
{
namespace render
{
	class RenderJob;
	struct RenderBatch;
	struct PipelineCommandInfo;
	class PipelineState;
	class Renderable;

	/**
	 * Type of a rendering command.
	 */
	struct PipelineCommandType
	{
		enum List
		{
			Invalid,
			/**
			 * Clears the current render target.
			 */
			Clear,
			/**
			 * Sets the current render target.
			 */
			SetTarget,
			/**
			 * Renders a single render job (for example for fullscreen quads).
			 */
			Batch,
			/**
			 * Renders the whole jobs submitted to the sequence using a certain
			 * context.
			 */
			BatchList,
			/**
			 * Binds a texture for all following batches.
			 */
			BindTexture,
			/**
			 * Unbind all previously bound textures.
			 */
			UnbindTextures,
			/**
			 * Recursively renders a list of other commands.
			 */
			CommandList
		};
	};
	/**
	 * Smallest part of a render pipeline, describes a single step in the
	 * rendering process.
	 */
	class PipelineCommand
	{
		public:
			/**
			 * Constructor.
			 * @param type Type of the command.
			 */
			PipelineCommand(PipelineCommandType::List type)
				: type(type)
			{
			}
			/**
			 * Destructor.
			 */
			virtual ~PipelineCommand()
			{
			}

			/**
			 * Prepares the pipeline command for rendering.
			 * @param renderer Renderer used for rendering.
			 * @param state Current pipeline state. Only valid for this single
			 * function call, will get destroyed shortly after.
			 * @param command Memory into which the command data is placed.
			 */
			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command) = 0;
			/**
			 * Finishes the pipeline command and makes it ready to be sent to
			 * the render thread if this is necessary. Only implemented for
			 * batch lists at the moment.
			 */
			virtual void endFrame()
			{
			}

			/**
			 * Returns the type of the command.
			 * @return Command type.
			 */
			PipelineCommandType::List getType()
			{
				return type;
			}
		private:
			PipelineCommandType::List type;
	};
	class ClearCommand : public PipelineCommand
	{
		public:
			ClearCommand()
				: PipelineCommand(PipelineCommandType::Clear), cleardepth(true),
				depth(1.0f)
			{
			}

			/**
			 * Sets whether and how the depth buffer shall be cleared.
			 * @param enable If true, the depth buffer is cleared.
			 * @param depth Depth with which the buffer is filled on clearing.
			 */
			void clearDepth(bool enable, float depth = 1.0f)
			{
				cleardepth = enable;
				this->depth = depth;
			}
			/**
			 * Sets whether and how a single color buffer shall be cleared.
			 * @param index Index of the color buffer in the render target.
			 * @param enable If true, the buffer is cleared.
			 * @param color Color used to fill the buffer with.
			 */
			void clearColor(unsigned int index, bool enable, core::Color color)
			{
				// Update existing entry
				for (unsigned int i = 0; i < this->color.size(); i++)
				{
					if (this->color[i].index == index)
					{
						if (enable)
						{
							this->color[i].color[0] = (float)color.getRed() / 255.0f;
							this->color[i].color[1] = (float)color.getGreen() / 255.0f;
							this->color[i].color[2] = (float)color.getBlue() / 255.0f;
							this->color[i].color[3] = (float)color.getAlpha() / 255.0f;
						}
						else
							this->color.erase(this->color.begin() + i);
						return;
					}
				}
				// Add new entry
				if (enable)
				{
					ClearColor clearentry;
					clearentry.index = index;
					clearentry.color[0] = (float)color.getRed() / 255.0f;
					clearentry.color[1] = (float)color.getGreen() / 255.0f;
					clearentry.color[2] = (float)color.getBlue() / 255.0f;
					clearentry.color[3] = (float)color.getAlpha() / 255.0f;
					this->color.push_back(clearentry);
				}
			}

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
		private:
			bool cleardepth;
			float depth;

			struct ClearColor
			{
				unsigned int index;
				float color[4];
			};
			std::vector<ClearColor> color;
			// TODO: Mutex?
	};
	class SetTargetCommand : public PipelineCommand
	{
		public:
			SetTargetCommand()
				: PipelineCommand(PipelineCommandType::SetTarget)
			{
			}

			/**
			 * Sets the target this command switches to.
			 * @param target Render target or 0 to render to the screen.
			 */
			void setTarget(RenderTarget::Ptr target)
			{
				this->target = target;
			}
			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
		private:
			RenderTarget::Ptr target;
	};
	class BindTextureCommand : public PipelineCommand
	{
		public:
			BindTextureCommand()
				: PipelineCommand(PipelineCommandType::BindTexture)
			{
			}

			/**
			 * Sets the name of the texture sampler in the shader which shall be
			 * set.
			 */
			void setName(const std::string &name)
			{
				// TODO: Use an int instead of string here
				this->name = name;
			}
			/**
			 * Sets the texture which shall be bound.
			 */
			void setTexture(Texture::Ptr texture)
			{
				this->texture = texture;
			}

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
		private:
			std::string name;
			Texture::Ptr texture;
	};
	class UnbindTexturesCommand : public PipelineCommand
	{
		public:
			UnbindTexturesCommand()
				: PipelineCommand(PipelineCommandType::UnbindTextures)
			{
			}

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
		private:
	};
	class BatchListCommand : public PipelineCommand
	{
		public:
			BatchListCommand()
				: PipelineCommand(PipelineCommandType::BatchList)
			{
			}

			/**
			 * Sets the context which shall be rendered for all materials
			 * supporting it.
			 * @param context Context name.
			 */
			void setContext(const std::string &context)
			{
				this->context = context;
			}
			/**
			 * Returns the context which is rendered in this command.
			 * @return Context name.
			 */
			std::string getContext()
			{
				return context;
			}

			/**
			 * Submits a renderable object to the batch list and prepares
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

			UniformData &getUniformData()
			{
				return uniform;
			}
			std::vector<DefaultUniform> &getDefaultUniforms()
			{
				return defuniform;
			}

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
			virtual void endFrame();
		private:
			void submit(RenderBatch *batch);

			std::string context;
			UniformData uniform;
			std::vector<DefaultUniform> defuniform;

			std::vector<RenderBatch*> batches;
			PipelineCommandInfo *info;
			Renderer *renderer;

			PipelineState *pipelinestate;
	};
	class BatchCommand : public PipelineCommand
	{
		public:
			BatchCommand();
			virtual ~BatchCommand();

			/**
			 * Sets the job rendered in this context.
			 * @param job Job to be rendered.
			 * @param context Context which is selected for rendering the job.
			 */
			void setJob(RenderJob *job, const std::string &context);

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
		private:
			RenderJob *job;
			std::string context;
	};
	class CommandList : public PipelineCommand
	{
		public:
			CommandList();
			virtual ~CommandList();

			void setName(const std::string &name)
			{
				this->name = name;
			}
			std::string getName()
			{
				return name;
			}

			void setActive(bool active);
			bool isActive();

			void addCommand(PipelineCommand *command);
			PipelineCommand *getCommand(unsigned int index);
			void removeCommand(unsigned int index);
			unsigned int getCommandCount();

			virtual void beginFrame(Renderer *renderer,
			                        PipelineState *state,
			                        PipelineCommandInfo *command);
			virtual void endFrame();
		private:
			bool active;
			std::string name;
			std::vector<PipelineCommand*> commands;
	};
}
}

#endif
