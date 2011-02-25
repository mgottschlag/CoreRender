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

#ifndef _CORERENDER_SCENE_SPOTLIGHT_HPP_INCLUDED_
#define _CORERENDER_SCENE_SPOTLIGHT_HPP_INCLUDED_

#include "Light.hpp"

namespace cr
{
namespace res
{
	class NameRegistry;
}
namespace scene
{
	class SpotLight : public Light
	{
		public:
			SpotLight(res::NameRegistry *names,
			          render::Material::Ptr deferredmat,
			          const char *lightcontext,
			          const char *shadowcontext);
			SpotLight(render::Material::Ptr deferredmat,
			          int lightcontext,
			          int shadowcontext);
			virtual ~SpotLight();

			void setRadius(float radius)
			{
				this->radius = radius;
			}
			float getRadius()
			{
				return radius;
			}
			void setAngle(float angle)
			{
				this->angle = angle;
			}
			float getAngle()
			{
				return angle;
			}

			virtual void prepareShadowMaps(render::SceneFrameData *frame,
			                               render::RenderQueue *queue,
			                               Camera::Ptr camera,
			                               math::Mat4f *shadowmat,
			                               render::LightUniforms *uniforms);
			virtual void getLightQuad(Camera::Ptr camera,
			                          float *quad);

			virtual void getLightInfo(render::LightUniforms *uniforms);

			typedef core::SharedPointer<SpotLight> Ptr;
		private:
			float radius;
			float angle;
	};
}
}

#endif
