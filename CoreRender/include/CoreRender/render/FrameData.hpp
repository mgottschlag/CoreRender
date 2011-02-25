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

#include "GameMath.hpp"
#include "../math/Frustum.hpp"
#include "FrameBuffer.hpp"

#include <tbb/spin_mutex.h>
#include <tbb/mutex.h>

namespace cr
{
namespace core
{
	class MemoryPool;
}
namespace render
{
	class RenderResource;
	class RenderObject;
	struct ShaderCombination;
	class VertexBuffer;
	class IndexBuffer;
	class RenderTarget;
	class Texture;
	class Material;
	class VertexLayout;
	struct RenderTargetInfo;

	struct CustomUniform
	{
		char *name;
		unsigned int size;
		float *data;
	};

	struct Batch
	{
		VertexBuffer *vertices;
		IndexBuffer *indices;
		VertexLayout *layout;

		ShaderCombination *shader;
		// TODO: Do we want the whole material here?
		Material *material;
		math::Mat4f transmat;

		unsigned int customuniformcount;
		CustomUniform *customuniforms;

		float sortkey;

		unsigned int startindex;
		unsigned int endindex;
		unsigned int basevertex;
		unsigned int vertexoffset;
		unsigned int indextype;
		// Skinning
		float *skinmat;
		unsigned int skinmatcount;
		// Instancing
		unsigned int transmatcount;
		math::Mat4f *transmatlist;
	};
	struct RenderCommandType
	{
		enum List
		{
			ClearTarget,
			SetTarget,
			RenderQueue,
			BindTextures,
			DrawQuad,
		};
	};
	struct TextureBinding
	{
		const char *name;
		Texture *tex;
	};
	struct LightUniforms
	{
		float position[4];
		float direction[3];
		float color[4];
		math::Mat4f shadowmat;
		Texture *shadowmap;
	};
	struct CameraUniforms
	{
		math::Mat4f viewmat;
		math::Mat4f projmat;
		math::Vec3f viewer;
	};
	struct RenderQueue
	{
		RenderQueue()
		{
			// TODO: Resize batches?
		}
		unsigned int context;
		CameraUniforms *camera;
		math::Frustum clipping[2];
		std::vector<Batch*> batches;
		tbb::mutex batchmutex;
		// TODO: Sort order
		/**
		 * Memory manager which is used for this render queue. We need this here
		 * so that we do not have an additional dependency on the FrameData
		 * everywhere where we use this.
		 */
		core::MemoryPool *memory;
		// TODO: Light uniforms
		LightUniforms *light;
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
				float depth;
			} cleartarget;
			struct
			{
				RenderTargetInfo *target;
				int viewport[4];
			} settarget;
			struct
			{
				RenderQueue *queue;
			} renderqueue;
			struct
			{
				unsigned int texturecount;
				TextureBinding *textures;
			} bindtextures;
			struct
			{
				/**
				 * Coordinates of the quad in screen space, coordinate values
				 * are in the range -1..1.
				 */
				float quad[4];
				ShaderCombination *shader;
				Material *material;
				CameraUniforms *camera;
				LightUniforms *light;
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

			RenderCommand *getFirstCommand()
			{
				return firstcommand;
			}

			RenderQueue *getRenderQueue(unsigned int index)
			{
				return &renderqueues[index];
			}
			RenderQueue *getRenderQueues()
			{
				return &renderqueues[0];
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
		struct ResourceUploadEntry
		{
			RenderResource *resource;
			void *data;
		};
		ResourceUploadEntry *resupload;
		unsigned int resuploadcount;
		RenderResource **resdeletion;
		unsigned int resdeletioncount;
		struct ObjectUploadEntry
		{
			RenderObject *object;
			void *data;
		};
		ObjectUploadEntry *objupload;
		unsigned int objuploadcount;
		RenderObject **objdeletion;
		unsigned int objdeletioncount;
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

			void addScene(SceneFrameData *scene)
			{
				tbb::spin_mutex::scoped_lock lock(scenemutex);
				scenes.push_back(scene);
			}
			const std::vector<SceneFrameData*> &getScenes()
			{
				return scenes;
			}

			UploadLists &getUploadLists()
			{
				return upload;
			}
			core::MemoryPool *getMemory()
			{
				return memory;
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
