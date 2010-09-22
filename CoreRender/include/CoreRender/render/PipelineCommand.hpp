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

class RenderBatch;

	struct PipelineCommandInfo;

	struct PipelineCommandType
	{
		enum List
		{
			Invalid,
			Clear,
			SetTarget,
			Batch,
			BatchList,
			BindTexture,
			FullscreenQuad,
			DeferredLightLoop,
			ForwardLightLoop,
			UnbindTextures,
			Sequence
		};
	};
	class PipelineCommand
	{
		public:
			PipelineCommand(PipelineCommandType::List type)
				: type(type)
			{
			}

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command) = 0;

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

			void clearDepth(bool enable, float depth = 1.0f)
			{
				cleardepth = enable;
				this->depth = depth;
			}
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

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
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

			void setTarget(RenderTarget::Ptr target)
			{
				this->target = target;
			}
			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
		private:
			RenderTarget::Ptr target;
	};
	class BindTextureCommand : public PipelineCommand
	{
		public:
			BindTextureCommand()
				: PipelineCommand(PipelineCommandType::BindTexture),
				name(-1)
			{
			}

			void setName(int name)
			{
				this->name = name;
			}
			void setTexture(Texture::Ptr texture)
			{
				this->texture = texture;
			}

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
		private:
			int name;
			Texture::Ptr texture;
	};
	class FullscreenQuadCommand : public PipelineCommand
	{
		public:
			FullscreenQuadCommand()
				: PipelineCommand(PipelineCommandType::FullscreenQuad)
			{
			}

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
		private:
			Material::Ptr material;
			std::string context;
	};
	class UnbindTexturesCommand : public PipelineCommand
	{
		public:
			UnbindTexturesCommand()
				: PipelineCommand(PipelineCommandType::UnbindTextures)
			{
			}

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
	private:
	};
	class SequenceCommand : public PipelineCommand
	{
		public:
			SequenceCommand()
				: PipelineCommand(PipelineCommandType::Sequence)
			{
			}

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
		private:
	};
	class BatchListCommand : public PipelineCommand
	{
		public:
			BatchListCommand()
				: PipelineCommand(PipelineCommandType::BatchList)
			{
			}

			void setContext(const std::string &context)
			{
				this->context = context;
			}
			std::string getContext()
			{
				return context;
			}

			void submit(RenderBatch *batch);

			void finish();

			virtual void apply(Renderer *renderer, PipelineCommandInfo *command);
		private:
			std::string context;

			std::vector<RenderBatch*> batches;
			PipelineCommandInfo *info;
			Renderer *renderer;
	};
}
}

#endif
