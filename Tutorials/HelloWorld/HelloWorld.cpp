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
	cr::res::ResourceManager *rmgr = graphics.getResourceManager();

	// Load some resources
	cr::render::Model::Ptr model = rmgr->getOrLoad<cr::render::Model>("Model",
	                                                                  "/models/jeep.model.xml");
	// Setup pipeline
	cr::render::Pipeline::Ptr pipeline = new cr::render::Pipeline();
	cr::render::RenderPass::Ptr pass = new cr::render::RenderPass("AMBIENT");
	pipeline->addPass(pass);
	graphics.addPipeline(pipeline);
	// Wait for resources to be loaded
	model->waitForLoading(true);
	// Model render job
	cr::math::Matrix4 matrix = cr::math::Matrix4::ScaleMat(cr::math::Vector3F(0.5f, 0.5f, 0.5f));
	std::vector<cr::render::RenderJob*> modeljobs(model->getMeshCount());
	for (unsigned int i = 0; i < model->getMeshCount(); i++)
	{
		modeljobs[i] = new cr::render::RenderJob;
		modeljobs[i]->vertices = model->getVertexBuffer();
		modeljobs[i]->indices = model->getIndexBuffer();
		cr::render::Model::Mesh *mesh = model->getMesh(i);
		cr::render::Model::Batch *batch = model->getBatch(mesh->batch);
		modeljobs[i]->material = mesh->material;
		modeljobs[i]->layout = batch->layout;
		modeljobs[i]->vertexcount = batch->vertexcount;
		modeljobs[i]->startindex = batch->startindex;
		modeljobs[i]->endindex = batch->startindex + batch->indexcount;
		modeljobs[i]->vertexoffset = batch->vertexoffset;
		modeljobs[i]->indextype = batch->indextype;
		modeljobs[i]->uniforms.add("worldMat") = matrix * mesh->transformation;
	}
	// Finished loading
	graphics.getLog()->info("Starting rendering.");
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
				default:
					break;
			}
		}
		// Begin frame
		graphics.beginFrame();
		// Render objects
		matrix = matrix * cr::math::Quaternion(cr::math::Vector3F(0.0, 0.1, 0.0)).toMatrix();
		for (unsigned int i = 0; i < modeljobs.size(); i++)
		{
			modeljobs[i]->uniforms["worldMat"] = matrix * model->getMesh(i)->transformation;
			pipeline->submit(modeljobs[i]);
		}
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
	for (unsigned int i = 0; i < modeljobs.size(); i++)
	{
		delete modeljobs[i];
	}
	pipeline = 0;
	pass = 0;
	model = 0;

	graphics.shutdown();
	return 0;
}