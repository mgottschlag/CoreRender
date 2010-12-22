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

#include "CoreRender/scene/AnimatedModel.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/core/MemoryPool.hpp"

#include <cstring>

namespace cr
{
namespace scene
{
	AnimatedModel::AnimatedModel(Model::Ptr model)
		: model(model)
	{
	}
	AnimatedModel::~AnimatedModel()
	{
	}

	void AnimatedModel::setModel(Model::Ptr model)
	{
		this->model = model;
		// Update animation bindings
		for (unsigned int i = 0; i < animstages.size(); i++)
		{
			animstages[i].binding = AnimationBinding(animstages[i].animation,
			                                         model);
		}
	}
	Model::Ptr AnimatedModel::getModel()
	{
		return model;
	}

	unsigned int AnimatedModel::addAnimation(Animation::Ptr animation,
	                                        float weight,
	                                        bool additive)
	{
		unsigned int oldanimcount = animstages.size();
		animstages.push_back(AnimationStage());
		AnimationStage &newstage = animstages[oldanimcount];
		newstage.animation = animation;
		newstage.weight = weight;
		newstage.additive = additive;
		newstage.binding = AnimationBinding(animation, model);
		newstage.time = 0.0f;
		return oldanimcount;
	}
	void AnimatedModel::setAnimation(unsigned int index, float time)
	{
		if (index >= animstages.size())
			return;
		animstages[index].time = time;
	}
	void AnimatedModel::removeAnimation(unsigned int index)
	{
		if (index >= animstages.size())
			return;
		animstages.erase(animstages.begin() + index);
	}
	unsigned int AnimatedModel::getAnimationCount()
	{
		return animstages.size();
	}

	void AnimatedModel::render(render::RenderQueue &queue,
	                          math::Matrix4 transmat)
	{
		// Compute animation data for all nodes
		std::vector<Model::Node> animatednodes = model->getNodes();
		applyAnimation(animatednodes);
		// Render mesh
		const std::vector<Model::Batch> &batches = model->getBatches();
		const std::vector<Model::BatchGeometry> &geometry = model->getGeometry();
		// TODO: Culling
		core::MemoryPool *memory = queue.memory;
		for (unsigned int i = 0; i < batches.size(); i++)
		{
			render::Batch *batch = model->prepareBatch(queue, i, false, true);
			if (!batch)
				continue;
			// Skinning
			if (batch->shader->skinning)
			{
				const Model::BatchGeometry *geom = &geometry[batches[i].geometry];
				unsigned int jointcount = geom->joints.size();
				void *ptr = memory->allocate(sizeof(float) * 16 * jointcount);
				float *skinmatrices = (float*)ptr;
				for (unsigned int j = 0; j < jointcount; j++)
				{
					int jointnode = geom->joints[j].node;
					if (jointnode == -1)
					{
						// TODO
						std::cout << "Invalid joint node." << std::endl;
					}
					else
					{
						math::Matrix4 jointmat = animatednodes[jointnode].abstrans * geom->joints[j].jointmat;
						memcpy(&skinmatrices[j * 16], jointmat.m, 16 * sizeof(float));
					}
				}
				batch->skinmat = skinmatrices;
				batch->skinmatcount = jointcount;
				batch->transmat = transmat;
				batch->transmatcount = 0;
			}
			else
			{
				batch->transmat = transmat * animatednodes[batches[i].node].abstrans;
				batch->transmatcount = 0;
			}
			// Add batch to the render queue
			tbb::mutex::scoped_lock lock(queue.batchmutex);
			queue.batches.push_back(batch);
		}
	}
	void AnimatedModel::render(render::RenderQueue &queue,
	                          unsigned int instancecount,
	                          math::Matrix4 *transmat)
	{
		core::MemoryPool *memory = queue.memory;
		// Create transformation matrix list
		unsigned int memsize = sizeof(math::Matrix4) * instancecount;
		math::Matrix4 *matrices = (math::Matrix4*)memory->allocate(memsize);
		for (unsigned int i = 0; i < instancecount; i++)
			matrices[i] = transmat[i];
		// Compute animation data for all nodes
		std::vector<Model::Node> animatednodes = model->getNodes();
		applyAnimation(animatednodes);
			// Render mesh
		const std::vector<Model::Batch> &batches = model->getBatches();
		const std::vector<Model::BatchGeometry> &geometry = model->getGeometry();
		// TODO: Culling
		for (unsigned int i = 0; i < batches.size(); i++)
		{
			render::Batch *batch = model->prepareBatch(queue, i, true, true);
			if (!batch)
				continue;
			// Skinning
			if (batch->shader->skinning)
			{
				const Model::BatchGeometry *geom = &geometry[batches[i].geometry];
				unsigned int jointcount = geom->joints.size();
				void *ptr = memory->allocate(sizeof(float) * 16 * jointcount);
				float *skinmatrices = (float*)ptr;
				for (unsigned int j = 0; j < jointcount; j++)
				{
					int jointnode = geom->joints[j].node;
					if (jointnode == -1)
					{
						// TODO
						std::cout << "Invalid joint node." << std::endl;
					}
					else
					{
						math::Matrix4 jointmat = animatednodes[jointnode].abstrans * geom->joints[j].jointmat;
						memcpy(&skinmatrices[j * 16], jointmat.m, 16 * sizeof(float));
					}
				}
				batch->skinmat = skinmatrices;
				batch->skinmatcount = jointcount;
				batch->transmat = math::Matrix4::Identity();
				batch->transmatcount = instancecount;
				batch->transmatlist = matrices;
			}
			else
			{
				batch->transmat = animatednodes[batches[i].node].abstrans;
				batch->transmatcount = instancecount;
				batch->transmatlist = matrices;
			}
			// Add batch to the render queue
			tbb::mutex::scoped_lock lock(queue.batchmutex);
			queue.batches.push_back(batch);
		}
	}

	float AnimatedModel::applyStage(unsigned int stageindex,
	                               AnimatedModel::NodeAnimationInfo *nodes,
	                               float weightsum)
	{
		float remainingweight = 1.0f - weightsum;
		AnimationStage &stage = animstages[stageindex];
		stage.binding.update();
		const std::vector<int> &binding = stage.binding.getNodes();
		// Get animation frame
		// TODO: Configurable interpolation
		//Animation::Frame *frame = stage.animation->getFrame(stage.time);
		Animation::Frame framedata;
		Animation::Frame *frame = &framedata;
		stage.animation->computeFrame(stage.time, framedata);
		// Apply frame
		if (stage.additive)
		{
			// TODO
			// Return 0 as the animation does not affect the weights of other
			// stages
			return 0.0f;
		}
		else
		{
			float weight = stage.weight * remainingweight;
			weight = weight / (weightsum + weight);
			for (unsigned int i = 0; i < frame->samples.size(); i++)
			{
				int node = binding[i];
				if (node == -1)
					continue;
				nodes[node].updated = true;
				// TODO: Properly interpolate here
				if (weight != 1.0)
				{
					nodes[node].trans.interpolate(nodes[node].trans,
					                              frame->samples[i].position,
					                              weight);
					nodes[node].scale.interpolate(nodes[node].scale,
					                              frame->samples[i].scale,
					                              weight);
					nodes[node].rot.interpolate(nodes[node].rot,
					                            frame->samples[i].rotation,
					                            weight);
				}
				else
				{
					nodes[node].trans = frame->samples[i].position;
					nodes[node].scale = frame->samples[i].scale;
					nodes[node].rot = frame->samples[i].rotation;
				}
			}
			// Return weight "consumed" by this stage
			float consumedweight = stage.weight * remainingweight;
			return consumedweight;
		}
	}

	void AnimatedModel::applyAnimation(std::vector<Model::Node> &nodes)
	{
		unsigned int nodecount = nodes.size();
		NodeAnimationInfo *animationinfo = new NodeAnimationInfo[nodecount];
		for (unsigned int i = 0; i < nodecount; i++)
			animationinfo[i].updated = false;
		float weightsum = 0.0f;
		for (unsigned int i = 0; i < animstages.size(); i++)
		{
			weightsum += applyStage(i, animationinfo, weightsum);
		}
		// Apply animation data
		for (unsigned int i = 0; i < nodecount; i++)
		{
			NodeAnimationInfo &animinfo = animationinfo[i];
			Model::Node &node = nodes[i];
			if (animinfo.updated)
			{
				node.transmat = math::Matrix4::TransMat(animinfo.trans)
				              * animinfo.rot.toMatrix()
				              * math::Matrix4::ScaleMat(animinfo.scale);
			}
		}
		// Compute absolute transformation
		for (unsigned int i = 0; i < nodecount; i++)
		{
			// Check whether absolute transformation needs to be updated
			bool dirty = false;
			if (animationinfo[i].updated)
				dirty = true;
			if (nodes[i].parent != -1
				&& animationinfo[nodes[i].parent].updated)
				dirty = true;
			if (!dirty)
				continue;
			// Update all children as well
			animationinfo[i].updated = true;
			// Update matrices
			if (nodes[i].parent == -1)
				nodes[i].abstrans = nodes[i].transmat;
			else
				nodes[i].abstrans = nodes[nodes[i].parent].abstrans
				                  * nodes[i].transmat;
		}
		delete[] animationinfo;
	}
}
}
