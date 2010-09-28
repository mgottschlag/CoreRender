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

#ifndef _CORERENDER_SCENE_SCENENODE_HPP_INCLUDED_
#define _CORERENDER_SCENE_SCENENODE_HPP_INCLUDED_

#include "CoreRender/math/Vector3.hpp"
#include "CoreRender/math/Quaternion.hpp"
#include "CoreRender/core/ReferenceCounted.hpp"
#include "CoreRender/render/Pipeline.hpp"

#include <vector>

namespace cr
{
namespace scene
{
	class SceneNode : public core::ReferenceCounted
	{
		public:
			SceneNode();
			virtual ~SceneNode();

			typedef core::SharedPointer<SceneNode> Ptr;

			void setPosition(const math::Vector3F &position);
			void setRotation(const math::Quaternion &rotation);
			void setRotation(const math::Vector3F &rotation);
			void setScale(const math::Vector3F &scale);
			math::Vector3F getPosition();
			math::Quaternion getRotation();
			math::Vector3F getScale();

			const math::Matrix4 &getTransMat()
			{
				return transmat;
			}
			const math::Matrix4 &getAbsTransMat()
			{
				return abstrans;
			}

			void setParent(SceneNode::Ptr parent);
			SceneNode::Ptr getParent();

			virtual void setVisible(bool visible);
			bool isVisible();

			void remove();

			void update(bool abstranschanged);
			void render(render::PipelineSequence *sequence);
		private:
			virtual void submit(render::PipelineSequence *sequence)
			{
			}
			virtual void onUpdate(bool abstranschanged)
			{
			}

			void removeChild(SceneNode::Ptr child);
			void addChild(SceneNode::Ptr child);

			SceneNode *parent;
			std::vector<SceneNode::Ptr> children;

			bool transmatdirty;
			math::Vector3F position;
			math::Quaternion rotation;
			math::Vector3F scale;
			math::Matrix4 transmat;
			math::Matrix4 abstrans;

			bool visible;

			tbb::spin_mutex mutex;
	};
}
}

#endif
