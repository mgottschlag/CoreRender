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

#ifndef _CORERENDER_RENDER_RENDERRESOURCE_HPP_INCLUDED_
#define _CORERENDER_RENDER_RENDERRESOURCE_HPP_INCLUDED_

#include "../res/Resource.hpp"

namespace cr
{
namespace render
{
	class Renderer;

	class RenderResource : public res::Resource
	{
		public:
			RenderResource(Renderer *renderer);
			~RenderResource();

			void waitForUpload();

			virtual bool create();
			virtual bool destroy();
			virtual bool upload();
		protected:
			Renderer *getRenderer()
			{
				return renderer;
			}

			void registerUpload();
			void uploadFinished();

			virtual void onDelete();
		private:
			Renderer *renderer;

			tbb::spin_mutex uploadmutex;
			bool uploading;
			core::ConditionVariable *waitvar;
	};
}
}

#endif
