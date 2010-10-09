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
	class PipelineStage;

	/**
	 * Current pipeline state which is used to keep track of bound textures
	 * for PipelineCommand::beginFrame().
	 */
	struct PipelineState
	{
		PipelineState(std::vector<DefaultUniform> &defuniforms)
			: defuniforms(defuniforms)
		{
		}
		std::map<std::string, Texture::Ptr> textures;
		std::vector<DefaultUniform> &defuniforms;
	};

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
			 * Returns the command tree which is executed in the render thread
			 * every time the pipeline is rendered.
			 */
			CommandList &getCommands()
			{
				return commands;
			}

			void addStage(CommandList *commands, const std::string &name);
			CommandList *getStage(const std::string &name);
			void removeStage(const std::string &name);
			unsigned int getStageCount();

			/**
			 * Adds a command list to the list of deferred light loops.This list
			 * will then hold the commands for shadow map generation and drawing
			 * the light.
			 * @note The list must be empty.
			 * @param stage Command list to be used as a light loop list.
			 */
			void addDeferredLightLoop(CommandList *lightloop);
			/**
			 * Returns the number of deferred light loops in this pipeline.
			 * @return Number of light loops.
			 */
			unsigned int getDeferredLightLoopCount();
			/**
			 * Returns a single command list which holds a deferred light loop.
			 * @param index Index of the light loop.
			 */
			CommandList *getDeferredLightLoop(unsigned int index);

			/**
			 * Adds a command list to the list of deferred light loops.This list
			 * will then hold the commands for shadow map generation and drawing
			 * all geometry again with lighting from the light.
			 * @note The list must be empty.
			 * @param stage Command list to be used as a light loop list.
			 */
			void addForwardLightLoop(CommandList *lightloop);
			/**
			 * Returns the number of forward light loops in this pipeline.
			 * @return Number of light loops.
			 */
			unsigned int getForwardLightLoopCount();
			/**
			 * Returns a single command list which holds a forward light loop.
			 * @param index Index of the light loop.
			 */
			CommandList *getForwardLightLoop(unsigned int index);

			void getBatchLists(std::vector<BatchListCommand*> &lists);

			/**
			 * Begins a new frame. Called by Renderer, do not call this
			 * manually.
			 * @param renderer Renderer which renders this frame.
			 * @param info Pipeline render info to be passed to the render
			 * thread.
			 */
			void beginFrame(Renderer *renderer, PipelineInfo *info);
			/**
			 * Prepares the pipeline data to be passed to the render thread and
			 * places it in info.
			 */
			void endFrame();

			/**
			 * Returns the default uniform values which are set for this
			 * pipeline. This is mostly used to set camera settings which are
			 * valid for all jobs rendered with this pipeline. This function
			 * returns a reference which can be used to add or modify uniform
			 * values.
			 * @return Reference to the default uniform values.
			 */
			std::vector<DefaultUniform> &getDefaultUniforms()
			{
				return uniforms;
			}

			typedef core::SharedPointer<Pipeline> Ptr;
		private:
			void getBatchLists(CommandList *commands,
			                   std::vector<BatchListCommand*> &lists);

			core::HashMap<std::string, CommandList*>::Type stages;

			std::vector<CommandList*> forwardlightloops;
			std::vector<CommandList*> deferredlightloops;

			CommandList commands;

			std::vector<DefaultUniform> uniforms;
	};
}
}

#endif
