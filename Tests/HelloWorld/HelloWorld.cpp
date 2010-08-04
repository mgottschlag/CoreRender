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

#include "CoreRender.hpp"

#include <iostream>
#include <GL/glfw.h>

int main(int argc, char **argv)
{
	// Initialize CoreRender
	cr::render::GraphicsEngine graphics;
	if (!graphics.init(cr::render::VideoDriverType::OpenGL, 800, 600, false))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	cr::res::ResourceManager *rmgr = graphics.getResourceManager();

	// Load some resources
	cr::render::Texture2D::Ptr texture = graphics.loadTexture2D("test.png");
	// TODO
	// Setup pipeline
	cr::render::Pipeline::Ptr pipeline = new cr::render::Pipeline();
	cr::render::RenderPass::Ptr pass = new cr::render::RenderPass("AMBIENT");
	pipeline->addPass(pass);
	graphics.addPipeline(pipeline);

	bool stopping = false;
	while (!stopping)
	{
		// Process input
		// TODO
		// Begin frame
		graphics.beginFrame();
		// Render objects
		// TODO
		// Finish and render frame
		graphics.endFrame();
	}

	graphics.shutdown();
	return 0;
}