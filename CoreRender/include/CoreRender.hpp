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
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/core/ReferenceCounted.hpp"
#include "CoreRender/core/File.hpp"
#include "CoreRender/core/Semaphore.hpp"
#include "CoreRender/core/Thread.hpp"
#include "CoreRender/core/FileSystem.hpp"
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
#include "CoreRender/render/RenderResource.hpp"
#include "CoreRender/render/RenderThread.hpp"
#include "CoreRender/render/RenderStats.hpp"
#include "CoreRender/render/RenderCaps.hpp"
#include "CoreRender/render/Shader.hpp"
#include "CoreRender/render/VideoDriverType.hpp"
#include "CoreRender/render/RenderContextOpenGL.hpp"
#include "CoreRender/render/RenderContextReuseOpenGL.hpp"
#include "CoreRender/render/GeometryManager.hpp"
#include "CoreRender/render/Texture.hpp"
#include "CoreRender/render/RenderContext.hpp"
#include "CoreRender/render/Material.hpp"
#include "CoreRender/render/RenderTarget.hpp"
#include "CoreRender/render/GraphicsEngine.hpp"
#include "CoreRender/render/ModelRenderable.hpp"
#include "CoreRender/render/PipelineSequence.hpp"
#include "CoreRender/render/PipelineStage.hpp"

/**
 * Main namespace containing the CoreRender classes.
 */
namespace cr
{
/**
 * @mainpage CoreRender
 *
 * @section about About
 * CoreRender is a shader-based multithreaded 3d engine designed for flexibility
 * and speed. It is modular and unlike other 3d engines does not contain any
 * scene graph, instead it uses submission style rendering (see
 * cr::render::Pipeline::submit()).
 *
 * @subsection quickstart Quick start
 * TBD
 *
 * @subsection example Example
 * TBD
 *
 * @subsection multithreading Multithreading
 * The engine uses two threads by default (unless told to use only one thread),
 * one thread dedicated to rendering and one which processes the scene and fills
 * the render queues (although this can be splitted up into multiple threads
 * again).
 *
 * This multithreading however needs the user of the engine to take care of some
 * things in order to maintain thread-safety as most of the engine by design is
 * not thread-safe.
 *
 * As resource loading happens in two steps (first new resources are created at
 * the beginning of a frame in the main thread, then they are uploaded in the
 * render thread), the user must take care not to create any resources after
 * cr::render::GraphicsEngine::beginFrame(), and must be careful when
 * manipulating resources which just have been queued for uploading the frame
 * before as one could then possible manipulate the resource already before it
 * was uploaded in the render thread. In the first case, the engine might not
 * render correctly as no handles to the newly created resource are available,
 * in the second the manipulated resource would be updated one frame too early
 * possibly causing wrong rendering as well. The latter can be prevented by
 * calling cr::render::RenderResource::waitForUpload() though which waits until
 * the upload of the resource has been finished.
 *
 * As a general guideline, if you call functions of the engine in parallel, only
 * do that with functions which are marked to be thread-safe. Even calling a
 * non-thread-safe function while at the same time calling a thread-safe
 * function might have side effects which let the engine crash.
 *
 * @subsection pipeline Rendering pipeline
 * The rendering pipeline in CoreRender is completely programmable. It mainly
 * consists of the two classes cr::render::Pipeline and cr::render::RenderPass.
 * While cr::render::GraphicsEngine holds multiple pipelines, each pipeline
 * again can hold more than one render pass.
 *
 * A pipeline defines a complete render operation, e.g. rendering the scene from
 * a camera. This can be done in multiple passes, where every pass is related to
 * a certain shader context. As an example, you might want to first render the
 * context "AMBIENT" for all renderable objects which draws all non-translucent
 * objects, then draw the context "TRANSLUCENT" which is only defined for your
 * translucent render jobs. In this case you would have one pipeline containing
 * two render passes. The first render pass additionally would have to dop a
 * clear operation to clear the screen before drawing. You might as well want to
 * have a second pipeline after that to render maybe a GUI, or a pipeline before
 * that to render an additional camera to a texture.
 *
 * Contexts are defined in cr::render::ShaderText and can have arbitrary names.
 * A shader can support a number of different contexts, so can e.g. the same
 * shader have contexts for forward and deferred rendering.
 */

/**
 * Namespace containing all 3d math classes.
 */
namespace math
{
}
/**
 * Namespace containing all core utility classes.
 */
namespace core
{
}
/**
 * Namespace containing all rendering classes.
 */
namespace render
{
}
/**
 * Namespace containing all resource management classes.
 */
namespace res
{
}
}

#endif
