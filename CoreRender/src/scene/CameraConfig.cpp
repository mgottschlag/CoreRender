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

#include "CoreRender/scene/CameraConfig.hpp"
#include <CoreRender/render/PipelineCommand.hpp>

namespace cr
{
namespace scene
{
	CameraConfig::CameraConfig()
		: frustum(math::Matrix4::Identity()), useboundingbox(false)
	{
		setProjection(math::Matrix4::Identity());
	}
	CameraConfig::~CameraConfig()
	{
	}

	void CameraConfig::setProjection(const math::Matrix4 &projmat)
	{
		this->projmat = projmat;
		frustum.setProjectionMatrix(projmat);
		// Set uniforms
		updateUniforms();
	}
	math::Matrix4 CameraConfig::getProjection()
	{
		return projmat;
	}

	void CameraConfig::setBoxCulling(bool active, const math::BoundingBox &box)
	{
		useboundingbox = active;
		boundingbox = box;
	}
	math::BoundingBox CameraConfig::getBoxCulling()
	{
		return boundingbox;
	}
	bool CameraConfig::getBoxCullingActive()
	{
		return useboundingbox;
	}

	void CameraConfig::addBatchList(render::BatchListCommand *batchlist)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		batchlists.push_back(batchlist);
		// Set uniforms
		render::DefaultUniform projuniform(render::DefaultUniformName::ProjMatrix,
		                                   projmat);
		batchlist->getDefaultUniforms().clear();
		batchlist->getDefaultUniforms().push_back(projuniform);
	}
	void CameraConfig::removeBatchList(render::BatchListCommand *batchlist)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		for (unsigned int i = 0; i < batchlists.size(); i++)
		{
			if (batchlists[i] == batchlist)
			{
				batchlists.erase(batchlists.begin() + i);
				return;
			}
		}
	}
	void CameraConfig::removeBatchList(unsigned int index)
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		if (index >= batchlists.size())
			return;
		batchlists.erase(batchlists.begin() + index);
	}
	render::BatchListCommand *CameraConfig::getBatchList(unsigned int index)
	{
		// TODO: Remove the mutex as this might be used very often from
		// different threads?
		tbb::spin_mutex::scoped_lock lock(mutex);
		if (index >= batchlists.size())
			return 0;
		return batchlists[index];
	}
	unsigned int CameraConfig::getBatchListCount()
	{
		tbb::spin_mutex::scoped_lock lock(mutex);
		return batchlists.size();
	}

	bool CameraConfig::cull(const math::BoundingBox &box)
	{
		// TODO
		return true;
	}

	void CameraConfig::updateUniforms()
	{
		render::DefaultUniform projuniform(render::DefaultUniformName::ProjMatrix,
		                                   projmat);
		for (unsigned int i = 0; i < batchlists.size(); i++)
		{
			// TODO: This can be optimized
			batchlists[i]->getDefaultUniforms().clear();
			batchlists[i]->getDefaultUniforms().push_back(projuniform);
		}
	}
}
}
