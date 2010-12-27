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

#include "CoreRender/scene/SpotLight.hpp"
#include "CoreRender/res/NameRegistry.hpp"
#include "CoreRender/render/FrameData.hpp"
#include "CoreRender/math/Quaternion.hpp"
#include "CoreRender/core/MemoryPool.hpp"

#include <cstring>

namespace cr
{
namespace scene
{
	SpotLight::SpotLight(res::NameRegistry *names,
	                     render::Material::Ptr deferredmat,
	                     const char *lightcontext,
	                     const char *shadowcontext)
		: radius(10.0f), angle(90.0f)
	{
		int lightctxhandle = -1;
		if (strcmp(lightcontext, ""))
			lightctxhandle = names->getContext(lightcontext);
		int shadowctxhandle = -1;
		if (strcmp(shadowcontext, ""))
			shadowctxhandle = names->getContext(shadowcontext);
		setMaterialSettings(deferredmat, lightctxhandle, shadowctxhandle);
	}
	SpotLight::SpotLight(render::Material::Ptr deferredmat,
	                     int lightcontext,
	                     int shadowcontext)
		: radius(10.0f)
	{
		setMaterialSettings(deferredmat, lightcontext, shadowcontext);
	}
	SpotLight::~SpotLight()
	{
	}

	void SpotLight::prepareShadowMaps(render::SceneFrameData *frame,
	                                  render::RenderQueue *queue,
	                                  Camera::Ptr camera,
	                                  math::Matrix4 *shadowmat,
	                                  render::LightUniforms *uniforms)
	{
		// Compute projection and view matrices of the shadow map camera
		math::Matrix4 projmat = math::Matrix4::PerspectiveFOV(getAngle(),
		                                                      1.0f,
		                                                      1.0f,
		                                                      getRadius());
		math::Vector3F rotation = (-getDirection()).getAngle();
		math::Matrix4 viewmat = math::Quaternion(rotation).toMatrix()
		                      * math::Matrix4::TransMat(-getPosition());
		*shadowmat = projmat * viewmat;
		// Create render queue
		queue->context = getShadowContext();
		queue->projmat = projmat;
		queue->viewmat = viewmat;
		queue->light = uniforms;
		// TODO: Clipping
		// Add draw command to the queue
		void *ptr = queue->memory->allocate(sizeof(render::RenderCommand));
		render::RenderCommand *cmd = (render::RenderCommand*)ptr;
		cmd->type = render::RenderCommandType::RenderQueue;
		cmd->renderqueue.queue = queue;
		frame->addCommand(cmd);
	}

	void SpotLight::getLightQuad(Camera::Ptr camera,
	                             float *quad)
	{
		// To find out the bounds of the light quad in screen space, we need
		// to transform the bounding box of the light
		// TODO: This is way too much
		math::BoundingBox lightbb(getPosition() + math::Vector3F(-radius, -radius, -radius),
		                          getPosition() + math::Vector3F(radius, radius, radius));
		lightbb.transform(camera->getViewMat());
		lightbb.transform(camera->getProjMat());
		quad[0] = lightbb.minCorner.x;
		quad[1] = lightbb.minCorner.y;
		quad[2] = lightbb.maxCorner.x;
		quad[3] = lightbb.maxCorner.y;
	}

	void SpotLight::getLightInfo(render::LightUniforms *uniforms)
	{
		Light::getLightInfo(uniforms);
		uniforms->position[3] = getRadius();
		uniforms->color[3] = cos(math::Math::degToRad(getAngle() * 0.5f));
	}
}
}
