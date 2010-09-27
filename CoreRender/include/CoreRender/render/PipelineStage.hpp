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

#ifndef _CORERENDER_RENDER_PIPELINESTAGE_HPP_INCLUDED_
#define _CORERENDER_RENDER_PIPELINESTAGE_HPP_INCLUDED_

#include <string>
#include <vector>

namespace cr
{
namespace render
{
	class PipelineCommand;
	class BatchListCommand;
	class Renderer;
	struct PipelineCommandInfo;
	class PipelineSequence;

	/**
	 * Class which holds several render commands which get executed at
	 * PipelineSequence::beginFrame().
	 */
	class PipelineStage
	{
		public:
			/**
			 * Constructor.
			 * @param name Name of the stage.
			 */
			PipelineStage(const std::string &name);
			/**
			 * Destructor.
			 */
			~PipelineStage();

			/**
			 * Adds a command at the end of the stage. The stage takes over
			 * ownership of the command and frees it when necessary.
			 */
			unsigned int addCommand(PipelineCommand *command);
			/**
			 * Returns the command at a certain index.
			 * @param index Index of the command.
			 * @return Command at the index.
			 */
			PipelineCommand *getCommand(unsigned int index);
			/**
			 * Removes the command at a certain index.
			 * @param index Index of the command.
			 */
			void removeCommand(unsigned int index);
			/**
			 * Returns the number of commands in this stage.
			 */
			unsigned int getCommandCount();

			/**
			 * Returns the batch list command at a certain index.
			 * @param index Index of the command.
			 * @return Batch list command.
			 */
			BatchListCommand *getBatchList(unsigned int index);
			/**
			 * Returns the number of BatchListCommand commands in the stage.
			 * @return Number of batch list commands.
			 */
			unsigned int getBatchListCount();

			/**
			 * Calls PipelineCommand::apply() for all commands. Do not call this
			 * directly.
			 * @param renderer Renderer used for rendering.
			 * @param sequence Sequence this stage belongs to.
			 * @param commands Memory buffer in which the commands are placed
			 * for rendering in the render thread.
			 */
			void beginFrame(Renderer *renderer,
			                PipelineSequence *sequence,
			                PipelineCommandInfo *commands);
			/**
			 * Finishes all batch list commands and prepares the buffers which
			 * are sent to the render thread.
			 */
			void finish();

			/**
			 * Returns the name of the stage.
			 */
			std::string getName();
		private:
			std::string name;

			std::vector<PipelineCommand*> commands;
			std::vector<BatchListCommand*> batchlists;
	};
}
}

#endif
