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

	class PipelineStage
	{
		public:
			PipelineStage(const std::string &name);
			~PipelineStage();

			unsigned int addCommand(PipelineCommand *command);
			PipelineCommand *getCommand(unsigned int index);
			void removeCommand(unsigned int index);
			unsigned int getCommandCount();

			BatchListCommand *getBatchList(unsigned int index);
			unsigned int getBatchListCount();

			void beginFrame(Renderer *renderer, PipelineCommandInfo *commands);
			void finish();

			std::string getName();
		private:
			std::string name;

			std::vector<PipelineCommand*> commands;
			std::vector<BatchListCommand*> batchlists;
	};
}
}

#endif
