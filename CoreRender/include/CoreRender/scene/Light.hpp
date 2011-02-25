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

#ifndef _CORERENDER_SCENE_LIGHT_HPP_INCLUDED_
#define _CORERENDER_SCENE_LIGHT_HPP_INCLUDED_

#include "../core/ReferenceCounted.hpp"
#include "../core/Color.hpp"
#include "../render/Material.hpp"
#include "Camera.hpp"

#include <GameMath.hpp>

namespace cr
{
namespace render
{
	struct LightUniforms;
	class SceneFrameData;
	struct RenderQueue;
}
namespace scene
{
	class Light : public core::ReferenceCounted
	{
		public:
			Light();
			virtual ~Light();

			void setColor(core::Color color)
			{
				this->color = color;
			}
			core::Color getColor()
			{
				return color;
			}

			void setPosition(math::Vec3f position)
			{
				this->position = position;
			}
			math::Vec3f getPosition()
			{
				return position;
			}

			void setDirection(math::Vec3f direction)
			{
				this->direction = direction;
			}
			math::Vec3f getDirection()
			{
				return direction;
			}

			render::Material::Ptr getDeferredMaterial()
			{
				return deferredmat;
			}
			int getLightContext()
			{
				return lightcontext;
			}
			int getShadowContext()
			{
				return shadowcontext;
			}

			void setShadowsEnabled(bool shadows)
			{
				shadowsenabled = shadows;
			}
			bool getShadowsEnabled()
			{
				return shadowsenabled;
			}

			unsigned int getShadowMapCount()
			{
				return shadowmapcount;
			}
			virtual void prepareShadowMaps(render::SceneFrameData *frame,
			                               render::RenderQueue *queue,
			                               Camera::Ptr camera,
			                               math::Mat4f *shadowmat,
			                               render::LightUniforms *uniforms);
			virtual void getLightQuad(Camera::Ptr camera,
			                          float *quad) = 0;

			virtual void getLightInfo(render::LightUniforms *uniforms);

			typedef core::SharedPointer<Light> Ptr;
		protected:
			void setMaterialSettings(render::Material::Ptr deferredmat,
			                         int lightcontext,
			                         int shadowcontext);

			void setShadowMapCount(unsigned int shadowmapcount)
			{
				this->shadowmapcount = shadowmapcount;
			}
		private:
			core::Color color;

			math::Vec3f position;
			math::Vec3f direction;

			render::Material::Ptr deferredmat;
			int lightcontext;
			int shadowcontext;

			bool shadowsenabled;

			unsigned int shadowmapcount;
	};
}
}

#endif
