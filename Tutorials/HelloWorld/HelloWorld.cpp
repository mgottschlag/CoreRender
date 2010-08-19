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

float vertices[24 * 8] = {
	// Front
	1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 1.0,
	-1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 1.0,
	-1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 0.0, 0.0,
	1.0, -1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0,
	// Back
	1.0, 1.0, -1.0, 0.0, 0.0, -1.0, 1.0, 1.0,
	1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 1.0, 0.0,
	-1.0, -1.0, -1.0, 0.0, 0.0, -1.0, 0.0, 0.0,
	-1.0, 1.0, -1.0, 0.0, 0.0, -1.0, 0.0, 1.0,
	// Top
	1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0,
	1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0,
	-1.0, 1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
	-1.0, 1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	// Bottom
	1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 1.0, 1.0,
	-1.0, -1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 1.0,
	-1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0,
	1.0, -1.0, -1.0, 0.0, 1.0, 0.0, 1.0, 0.0,
	// Left
	1.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0,
	1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 1.0,
	1.0, -1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0,
	1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 0.0,
	// Right +
	-1.0, 1.0, 1.0, -1.0, 0.0, 0.0, 1.0, 1.0,
	-1.0, 1.0, -1.0, -1.0, 0.0, 0.0, 1.0, 0.0,
	-1.0, -1.0, -1.0, -1.0, 0.0, 0.0, 0.0, 0.0,
	-1.0, -1.0, 1.0, -1.0, 0.0, 0.0, 0.0, 1.0,
};

unsigned short indices[36] = {
	// Front
	0, 1, 2,
	0, 2, 3,
	// Back
	4, 5, 6,
	4, 6, 7,
	// Top
	8, 9, 10,
	8, 10, 11,
	// Bottom
	12, 13, 14,
	12, 14, 15,
	// Left
	16, 17, 18,
	16, 18, 19,
	// Right
	20, 21, 22,
	20, 22, 23,
};

int main(int argc, char **argv)
{
	cr::render::GraphicsEngine graphics;
	// Initialize file system
	{
		cr::core::StandardFileSystem::Ptr filesystem;
		filesystem = new cr::core::StandardFileSystem();
		filesystem->mount("Tutorials/media/", "/", cr::core::FileAccess::Read);
		// The following lines do not work with out-of-tree builds
		//std::string rootdir = cr::core::FileSystem::getDirectory(argv[0]);
		//filesystem->mount(rootdir + "/../media/", "/", cr::core::FileAccess::Read);
		// Writes (e.g. log) shall go into the working directory
		filesystem->mount("", "/", cr::core::FileAccess::Write);
		graphics.setFileSystem(filesystem);
	}
	// Initialize CoreRender
	if (!graphics.init(cr::render::VideoDriverType::OpenGL, 800, 600, false, 0, true))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(cr::core::LogLevel::Debug);

	// Load some resources
	cr::render::VertexBuffer::Ptr vb = graphics.createVertexBuffer();
	vb->set(24 * 8 * sizeof(float), vertices);
	cr::render::IndexBuffer::Ptr ib = graphics.createIndexBuffer();
	ib->set(36 * sizeof(unsigned short), indices);
	cr::render::Material::Ptr material = graphics.loadMaterial("/materials/Simple.material.xml");
	// Create vertex layout
	cr::render::VertexLayout::Ptr layout = new cr::render::VertexLayout(3);
	layout->setElement(0, "pos", 0, 3, 0, cr::render::VertexElementType::Float, 32);
	layout->setElement(1, "normal", 0, 3, 12, cr::render::VertexElementType::Float, 32);
	layout->setElement(2, "texcoord", 0, 2, 24, cr::render::VertexElementType::Float, 32);
	// Create render job
	cr::render::RenderJob *job = new cr::render::RenderJob;
	job->vertices = vb;
	job->vertexcount = 24;
	job->indices = ib;
	job->endindex = 36;
	job->material = material;
	job->layout = layout;
	// This does not work as the shader is not loaded yet
	//job->uniforms = shader->getUniformData();
	cr::math::Matrix4 matrix = cr::math::Matrix4::ScaleMat(cr::math::Vector3F(0.5f, 0.5f, 0.5f));
	job->uniforms.add("worldMat") = matrix;
	// Setup pipeline
	cr::render::Pipeline::Ptr pipeline = new cr::render::Pipeline();
	cr::render::RenderPass::Ptr pass = new cr::render::RenderPass("AMBIENT");
	pipeline->addPass(pass);
	graphics.addPipeline(pipeline);
	// Wait for resources to be loaded
	material->waitForLoading(true);
	// Render loop
	bool stopping = false;
	uint64_t fpstime = cr::core::Time::getSystemTime();
	int fps = 0;
	while (!stopping)
	{
		// Process input
		cr::render::InputEvent input;
		while (graphics.getInput(&input))
		{
			switch (input.type)
			{
				case cr::render::InputEventType::WindowClosed:
					stopping = true;
					break;
				case cr::render::InputEventType::KeyDown:
					/*if (input.keyboard.key == cr::render::KeyCode::Up)
						scale.y *= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Down)
						scale.y /= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Left)
						scale.x *= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Right)
						scale.x /= 1.1f;
					job->uniforms["scale"] = scale;*/
					break;
				default:
					break;
			}
		}
		// Begin frame
		graphics.beginFrame();
		// Render objects
		matrix = matrix * cr::math::Quaternion(cr::math::Vector3F(0.0, 0.1, 0.1)).toMatrix();
		job->uniforms["worldMat"] = matrix;
		pipeline->submit(job);
		// Finish and render frame
		graphics.endFrame();
		fps++;
		uint64_t currenttime = cr::core::Time::getSystemTime();
		if (currenttime - fpstime >= 1000000)
		{
			std::cout << "FPS: " << fps << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}

	// Delete resources
	delete job;
	vb = 0;
	ib = 0;
	material = 0;
	pipeline = 0;
	pass = 0;
	// TODO: Other resources

	graphics.shutdown();
	return 0;
}