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

#ifndef _CORERENDER_RENDER_FRAMEDATA_HPP_INCLUDED_
#define _CORERENDER_RENDER_FRAMEDATA_HPP_INCLUDED_

#include "../math/Matrix4.hpp"
#include "../math/Frustum.hpp"

#include <tbb/spin_mutex.h>

namespace cr
{
namespace core
{
	class MemoryPool;
}
namespace render
{
	class RenderResource;
	class ShaderCombination;
	class VertexBuffer;
	class IndexBuffer;
	class RenderTarget;
	class Texture;
	class Material;

	struct Batch
	{
		VertexBuffer *vertices;
		IndexBuffer *indices;
		ShaderCombination *shader;
		// TODO: Do we want the whole material here?
		Material *material;
		math::Matrix4 transmat;
		// TODO: Custom uniforms
		float sortkey;
		unsigned int startindex;
		unsigned int endindex;
		unsigned int basevertex;
		unsigned int vertexoffset;
		unsigned int indextype;
	};
	struct RenderCommandType
	{
		enum List
		{
			// TODO
			ClearTarget,
			SetTarget,
			RenderQueue,
			BindTexture,
			UnbindTextures,
			DrawQuad,
		};
	};
	struct RenderQueue
	{
		RenderTarget *target;
		unsigned int context;
		math::Matrix4 projmat;
		math::Matrix4 viewmat;
		math::Frustum clipping[2];
		int viewport[4];
		// TODO: Do we want a std::vector here?
		Batch *batches;
		unsigned int batchcount;
		// TODO: Try tbb::mutex here
		tbb::spin_mutex batchmutex;
		// TODO: Sort order
		/**
		 * Memory manager which is used for this render queue. We need this here
		 * so that we do not have an additional dependency on the FrameData
		 * everywhere where we use this.
		 */
		core::MemoryPool *memory;
	};
	struct RenderCommand
	{
		RenderCommandType::List type;
		union
		{
			struct
			{
				unsigned int buffers;
				float color[4];
			} cleartarget;
			struct
			{
				RenderTarget *target;
			} settarget;
			struct
			{
				RenderQueue *queue;
			} renderqueue;
			struct
			{
				Texture *texture;
				const char *sampler;
			} bindtexture;
			struct
			{
				/**
				 * Coordinates of the quad in screen space, coordinate values
				 * are in the range -1..1.
				 */
				float quad[4];

			} drawquad;
		};
		RenderCommand *next;
	};
	class SceneFrameData
	{
		public:
			SceneFrameData(RenderQueue *renderqueues, unsigned int renderqueuecount)
				: renderqueues(renderqueues), renderqueuecount(renderqueuecount),
				firstcommand(0), lastcommand(0)
			{
			}

			void addCommand(RenderCommand *command)
			{
				command->next = 0;
				if (lastcommand)
				{
					lastcommand->next = command;
					lastcommand = command;
				}
				else
				{
					firstcommand = command;
					lastcommand = command;
				}
			}

			RenderQueue *getRenderQueue(unsigned int index)
			{
				return &renderqueues[index];
			}
			unsigned int getRenderQueueCount()
			{
				return renderqueuecount;
			}
		private:
			RenderQueue *renderqueues;
			unsigned int renderqueuecount;
			RenderCommand *firstcommand;
			RenderCommand *lastcommand;
	};
	struct UploadLists
	{
		struct UploadEntry
		{
			RenderResource *resource;
			void *data;
		};
		UploadEntry *upload;
		unsigned int uploadcount;
		RenderResource *deletion;
		unsigned int deletioncount;
	};
	class FrameData
	{
		public:
			FrameData(core::MemoryPool *memory)
				: memory(memory)
			{
			}

			/**
			 * Sorts all batches to maximize rendering performance.
			 */
			void optimize();

			UploadLists &getUploadLists()
			{
				return upload;
			}
		private:
			core::MemoryPool *memory;
			UploadLists upload;
			std::vector<SceneFrameData*> scenes;
			// TODO: Try tbb::mutex here
			tbb::spin_mutex scenemutex;
	};
}
}

#endif
