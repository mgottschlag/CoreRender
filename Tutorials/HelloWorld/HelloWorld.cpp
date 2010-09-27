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
	                                                                  "/models/dwarf.model.xml");
	cr::render::Animation::Ptr anim = rmgr->getOrLoad<cr::render::Animation>("Animation",
	                                                                         "/models/dwarf.anim");
	// Load pipeline
	cr::render::PipelineDefinition::Ptr pipelinedef;
	pipelinedef = rmgr->getOrLoad<cr::render::PipelineDefinition>("PipelineDefinition",
	                                                              "/pipelines/Simple.pipeline.xml");
	pipelinedef->waitForLoading(true);
	cr::render::Pipeline::Ptr pipeline = pipelinedef->createPipeline();
	graphics.addPipeline(pipeline);
	// Setup camera matrix
	cr::math::Matrix4 projmat = cr::math::Matrix4::PerspectiveFOV(60.0f, 4.0f/3.0f, 1.0f, 1000.0f);
	projmat = projmat * cr::math::Matrix4::TransMat(cr::math::Vector3F(0, 0, -100));
	projmat = projmat * cr::math::Quaternion(cr::math::Vector3F(45.0, 0.0, 0.0)).toMatrix();
	pipeline->getDefaultSequence()->getDefaultUniforms().push_back(cr::render::DefaultUniform(cr::render::DefaultUniformName::ProjMatrix, projmat));
	// Wait for resources to be loaded
	model->waitForLoading(true);
	anim->waitForLoading(true);
	// Create renderable
	cr::render::ModelRenderable *renderable = new cr::render::ModelRenderable();
	renderable->setModel(model);
	renderable->setTransMat(cr::math::Quaternion(cr::math::Vector3F(0.0, 45.0, 0.0)).toMatrix());
	renderable->addAnimStage(anim, 1.0);
	// Finished loading
	graphics.getLog()->info("Starting rendering.");
	// Render loop
	bool stopping = false;
	cr::core::Time fpstime = cr::core::Time::Now();
	int fps = 0;
	float rotation = 0.0f;
	float animtime = 0.0f;
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
		animtime += 0.001f;
		if (animtime >= anim->getFrameCount() / anim->getFramesPerSecond())
			animtime -= anim->getFrameCount() / anim->getFramesPerSecond();
		renderable->getAnimStage(0)->time = animtime;
		rotation += 0.1f;
		cr::math::Quaternion quat(cr::math::Vector3F(0.0, rotation, 0.0));
		renderable->setTransMat(quat.toMatrix());
		pipeline->getDefaultSequence()->submit(renderable);
		// Finish and render frame
		graphics.endFrame();
		fps++;
		cr::core::Time currenttime = cr::core::Time::Now();
		if (currenttime - fpstime >= cr::core::Duration::Seconds(1))
		{
			std::cout << "FPS: " << fps << std::endl;
			const cr::render::RenderStats &stats = graphics.getRenderStats();
			std::cout << stats.getPolygonCount() << " triangles, ";
			std::cout << stats.getBatchCount() << " batches." << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}

	// Delete resources
	delete renderable;
	pipeline = 0;
	model = 0;
	anim = 0;
	pipelinedef = 0;

	graphics.shutdown();
	return 0;
}
