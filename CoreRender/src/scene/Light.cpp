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

#include "CoreRender/scene/Light.hpp"
#include "CoreRender/render/FrameData.hpp"

namespace cr
{
namespace scene
{
	Light::Light()
		: color(1.0, 1.0, 1.0, 1.0), position(0, 0, 0), direction(0, 0, 1),
		lightcontext(-1), shadowcontext(-1), shadowsenabled(false),
		shadowmapcount(1)
	{
	}
	Light::~Light()
	{
	}

	void Light::setMaterialSettings(render::Material::Ptr deferredmat,
	                                int lightcontext,
	                                int shadowcontext)
	{
		this->deferredmat = deferredmat;
		this->lightcontext = lightcontext;
		this->shadowcontext = shadowcontext;
	}

	void Light::prepareShadowMaps(render::SceneFrameData *frame,
	                              render::RenderQueue *queue,
	                              Camera::Ptr camera,
	                              math::Mat4f *shadowmat,
	                              render::LightUniforms *uniforms)
	{
	}

	void Light::getLightInfo(render::LightUniforms *uniforms)
	{
		uniforms->color[0] = color.getRed();
		uniforms->color[1] = color.getGreen();
		uniforms->color[2] = color.getBlue();
		uniforms->color[3] = color.getAlpha();
		uniforms->position[0] = getPosition().x;
		uniforms->position[1] = getPosition().y;
		uniforms->position[2] = getPosition().z;
		uniforms->position[3] = 1.0f;
		uniforms->direction[0] = getDirection().x;
		uniforms->direction[1] = getDirection().y;
		uniforms->direction[2] = getDirection().z;
	}
}
}
