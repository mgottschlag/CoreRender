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

#ifndef _CORERENDER_HPP_INCLUDED_
#define _CORERENDER_HPP_INCLUDED_

#include "CoreRender/core/Functor.hpp"
#include "CoreRender/core/StandardFile.hpp"
#include "CoreRender/core/FileList.hpp"
#include "CoreRender/core/FrameLimiter.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/core/ReferenceCounted.hpp"
#include "CoreRender/core/File.hpp"
#include "CoreRender/core/Semaphore.hpp"
#include "CoreRender/core/Thread.hpp"
#include "CoreRender/core/FileSystem.hpp"
#include "CoreRender/core/Buffer.hpp"
#include "CoreRender/core/Hardware.hpp"
#include "CoreRender/core/StandardFileSystem.hpp"
#include "CoreRender/core/Time.hpp"
#include "CoreRender/core/Log.hpp"
#include "CoreRender/math/Vector3.hpp"
#include "CoreRender/math/Alignment.hpp"
#include "CoreRender/math/Quaternion.hpp"
#include "CoreRender/math/Matrix4.hpp"
#include "CoreRender/math/Math.hpp"
#include "CoreRender/math/Vector2.hpp"
#include "CoreRender/math/ScreenPosition.hpp"
#include "CoreRender/math/StdInt.hpp"
#include "CoreRender/res/LoadingThread.hpp"
#include "CoreRender/res/Resource.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/render/Animation.hpp"
#include "CoreRender/render/RenderBatch.hpp"
#include "CoreRender/render/RenderResource.hpp"
#include "CoreRender/render/RenderThread.hpp"
#include "CoreRender/render/RenderStats.hpp"
#include "CoreRender/render/RenderCaps.hpp"
#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/VideoDriverType.hpp"
#include "CoreRender/render/RenderContextOpenGL.hpp"
#include "CoreRender/render/RenderContextReuseOpenGL.hpp"
#include "CoreRender/render/GeometryManager.hpp"
#include "CoreRender/render/RenderPass.hpp"
#include "CoreRender/render/Texture.hpp"
#include "CoreRender/render/RenderContext.hpp"
#include "CoreRender/render/Material.hpp"
#include "CoreRender/render/RenderTarget.hpp"
#include "CoreRender/render/GraphicsEngine.hpp"
#include "CoreRender/render/ModelRenderable.hpp"

namespace cr
{
}

#endif
