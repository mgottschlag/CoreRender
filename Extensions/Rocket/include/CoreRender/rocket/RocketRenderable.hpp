/*
 * Copyright (C) 2011, Mathias Gottschlag
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _CORERENDER_ROCKET_ROCKETRENDERABLE_HPP_INCLUDED_
#define _CORERENDER_ROCKET_ROCKETRENDERABLE_HPP_INCLUDED_

#include "SystemInterface.hpp"
#include "FileInterface.hpp"
#include "CoreRender/core/ReferenceCounted.hpp"
#include "CoreRender/render/Mesh.hpp"
#include "CoreRender/render/Material.hpp"

#include <GameMath.hpp>

namespace cr
{
class GraphicsEngine;
namespace render
{
	class RenderQueue;
}
namespace rocket
{
	class RenderInterface;
	class RocketRenderable : public core::ReferenceCounted
	{
		public:
			RocketRenderable(GraphicsEngine *graphics,
			                 render::Material::Ptr texturematerial,
			                 render::Material::Ptr colormaterial);
			virtual ~RocketRenderable();

			void reset();
			void render(render::RenderQueue &queue,
			            math::Mat4f transmat);

			void renderMesh(render::Mesh::Ptr mesh,
			                render::Material::Ptr material,
			                math::Vec2i translation);

			void setScissorsEnabled(bool enabled);
			void setScissorRegion(int x, int y, int width, int height);

			RenderInterface *getRenderInterface()
			{
				return renderinterface;
			}
			SystemInterface *getSystemInterface()
			{
				return &systeminterface;
			}
			FileInterface *getFileInterface()
			{
				return &fileinterface;
			}

			typedef core::SharedPointer<RocketRenderable> Ptr;
		private:
			RenderInterface *renderinterface;
			SystemInterface systeminterface;
			FileInterface fileinterface;

			struct ScissorRegion
			{
				unsigned int x;
				unsigned int y;
				unsigned int width;
				unsigned int height;
			};
			std::vector<ScissorRegion> scissorentries;
			bool scissorenabled;

			struct RenderEntry
			{
				render::Mesh::Ptr mesh;
				render::Material::Ptr material;
				math::Vec2i translation;
				int scissor;
			};

			std::vector<RenderEntry> renderentries;
	};
}
}

#endif
