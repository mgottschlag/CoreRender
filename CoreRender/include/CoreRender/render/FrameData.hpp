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

#include "FrameBuffer.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "../core/MemoryPool.hpp"
#include "CoreRender/core/Time.hpp"

#include <GameMath.hpp>
#include <tbb/spin_mutex.h>
#include <tbb/mutex.h>
#include <cstring>
#include <cstdlib>

namespace cr
{
namespace core
{
	class MemoryPool;
}
namespace render
{
	class RenderResource;
	struct ShaderCombination;
	class RenderTarget;
	struct RenderTargetInfo;

	struct CustomUniform
	{
		char *name;
		unsigned int size;
		float *data;
	};

	struct ScissorRect
	{
		unsigned int x;
		unsigned int y;
		unsigned int width;
		unsigned int height;
	};

	struct Batch
	{
		// Draw info
		Mesh *mesh;
		ShaderCombination *shader;
		Material *material;
		math::Mat4f transmat;
		unsigned int customuniformcount;
		CustomUniform *customuniforms;

		// Used for optimization (front-to-back, back-to-front or state change
		// sorting)
		float sortkey;

		// Skinning
		float *skinmat;
		unsigned int skinmatcount;
		// Instancing
		unsigned int transmatcount;
		math::Mat4f *transmatlist;

		// Scissor rect (only used when non-null, mostly used for 2d rendering)
		ScissorRect *scissor;
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

		Batch *prepareBatch(Material *material,
		                    Mesh *mesh,
		                    bool instancing = false,
		                    bool skinning = false)
		{
			// Get shader
			if (!material->getShader())
				return 0;
			unsigned int shaderflagmask;
				unsigned int shaderflagvalue;
			material->getShaderFlags(shaderflagmask,
			                         shaderflagvalue);
			render::ShaderCombination *shader;
			shader = material->getShader()->getCombination(context,
			                                               shaderflagmask,
			                                               shaderflagvalue,
			                                               instancing,
			                                               skinning).get();
			if (!shader)
				return 0;
			// Create batch
			void *ptr = memory->allocate(sizeof(render::Batch));
			render::Batch *batch = (render::Batch*)ptr;
			batch->mesh = mesh;
			batch->shader = shader;
			batch->material = material;
			batch->transmatcount = 0;
			batch->skinmat = 0;
			batch->skinmatcount = 0;
			// TODO: Sorting
			batch->sortkey = 0.0f;
			// Custom uniforms
			const std::vector<render::Material::UniformInfo> &uniforms = material->getUniforms();
			batch->customuniformcount = uniforms.size();
			if (uniforms.size() > 0)
			{
				ptr = memory->allocate(sizeof(render::CustomUniform) * uniforms.size());
				render::CustomUniform *uniformdata = (render::CustomUniform*)ptr;
				for (unsigned int i = 0; i < uniforms.size(); i++)
				{
					uniformdata[i].size = uniforms[i].size;
					uniformdata[i].data = (float*)memory->allocate(sizeof(float) * uniforms[i].size);
					std::memcpy(uniformdata[i].data,
					            uniforms[i].data,
					            uniforms[i].size * sizeof(float));
					uniformdata[i].name = (char*)memory->allocate(uniforms[i].name.length() + 1);
					std::strcpy(uniformdata[i].name, uniforms[i].name.c_str());
				}
				batch->customuniforms = uniformdata;
			}
			else
				batch->customuniforms = 0;
			// Scissors
			batch->scissor = 0;
			return batch;
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
				composestarttime = core::Time::Now();
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

			void endFrame()
			{
				composeendtime = core::Time::Now();
			}
			core::Time getComposeStartTime()
			{
				return composestarttime;
			}
			core::Time getComposeEndTime()
			{
				return composeendtime;
			}
	private:
			core::MemoryPool *memory;
			UploadLists upload;
			std::vector<SceneFrameData*> scenes;
			// TODO: Try tbb::mutex here
			tbb::spin_mutex scenemutex;

			core::Time composestarttime;
			core::Time composeendtime;
	};
}
}

#endif
