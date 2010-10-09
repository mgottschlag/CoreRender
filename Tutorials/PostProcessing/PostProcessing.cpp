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

using namespace cr;

int main(int argc, char **argv)
{
	render::GraphicsEngine graphics;
	// Initialize file system
	{
		core::StandardFileSystem::Ptr filesystem;
		filesystem = new core::StandardFileSystem();
		filesystem->mount("Tutorials/media/", "/", core::FileAccess::Read);
		// The following lines do not work with out-of-tree builds
		//std::string rootdir = core::FileSystem::getDirectory(argv[0]);
		//filesystem->mount(rootdir + "/../media/", "/", core::FileAccess::Read);
		// Writes (e.g. log) shall go into the working directory
		filesystem->mount("", "/", core::FileAccess::Write);
		graphics.setFileSystem(filesystem);
	}
	// Initialize CoreRender
	if (!graphics.init(render::VideoDriverType::OpenGL, 800, 600, false, 0, true))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(core::LogLevel::Debug);
	res::ResourceManager *rmgr = graphics.getResourceManager();
	// Load some resources
	render::Model::Ptr model = rmgr->getOrLoad<render::Model>("Model",
	                                                          "/models/dwarf.model.xml");
	render::Animation::Ptr anim = rmgr->getOrLoad<render::Animation>("Animation",
	                                                                 "/models/dwarf.anim");
	render::Model::Ptr cube = rmgr->getOrLoad<render::Model>("Model",
	                                                         "/models/cube.model.xml");
	// Load pipeline
	render::PipelineDefinition::Ptr pipelinedef;
	pipelinedef = rmgr->getOrLoad<render::PipelineDefinition>("PipelineDefinition",
	                                                          "/pipelines/Blur.pipeline.xml");
	pipelinedef->waitForLoading(true);
	render::Pipeline::Ptr pipeline = pipelinedef->createPipeline(800, 600);
	graphics.addPipeline(pipeline);
	std::vector<render::BatchListCommand*> batchlists;
	pipeline->getBatchLists(batchlists);
	// Set target size
	{
		render::Material::Ptr blurmaterial = rmgr->getOrLoad<render::Material>("Material",
		                                                                       "/materials/Blur.material.xml");
		blurmaterial->getUniformData().add("targetSize") = math::Vector2F(400, 300);
	}
	// Setup camera matrix for the scene
	{
		math::Matrix4 projmat = math::Matrix4::Ortho(100.0f, 100.0f, -100.0f, 100.0f);
		projmat = projmat * math::Matrix4::TransMat(math::Vector3F(0, -40, 0));
		render::DefaultUniform projuniform(render::DefaultUniformName::ProjMatrix, projmat);
		pipeline->getDefaultUniforms().push_back(projuniform);
	}
	// Create renderable
	render::ModelRenderable *renderable = new render::ModelRenderable();
	renderable->setModel(model);
	renderable->setTransMat(math::Matrix4::TransMat(math::Vector3F(0.0f, -40.0f, 0.0f)));
	renderable->addAnimStage(anim, 1.0);
	// Wait for resources to be loaded
	model->waitForLoading(true);
	anim->waitForLoading(true);
	// Finished loading
	graphics.getLog()->info("Starting rendering.");
	// Render loop
	bool stopping = false;
	core::Time fpstime = core::Time::Now();
	int fps = 0;
	float rotation = 0.0f;
	float animtime = 0.0f;
	while (!stopping)
	{
		// Process input
		render::InputEvent input;
		while (graphics.getInput(&input))
		{
			switch (input.type)
			{
				case render::InputEventType::WindowClosed:
					stopping = true;
					break;
				default:
					break;
			}
		}
		// Begin frame
		graphics.beginFrame();
		// Render scene to texture
		animtime += 0.001f;
		if (animtime >= anim->getFrameCount() / anim->getFramesPerSecond())
			animtime -= anim->getFrameCount() / anim->getFramesPerSecond();
		renderable->getAnimStage(0)->time = animtime;
		rotation += 0.1f;
		math::Quaternion quat(math::Vector3F(0, rotation, 0));
		renderable->setTransMat(quat.toMatrix());
		// Render model
		for (unsigned int i = 0; i < batchlists.size(); i++)
			batchlists[i]->submit(renderable);
		// Finish and render frame
		graphics.endFrame();
		fps++;
		core::Time currenttime = core::Time::Now();
		if (currenttime - fpstime >= core::Duration::Seconds(1))
		{
			std::cout << "FPS: " << fps << std::endl;
			const render::RenderStats &stats = graphics.getRenderStats();
			std::cout << stats.getPolygonCount() << " triangles, ";
			std::cout << stats.getBatchCount() << " batches." << std::endl;
			std::cout << "Time: " << stats.getRenderTime().getNanoseconds()
				<< " rendering, " << stats.getWaitTime().getNanoseconds()
				<< " waiting." << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}

	// Delete resources
	delete renderable;
	model = 0;
	anim = 0;
	cube = 0;
	pipelinedef = 0;
	pipeline = 0;

	graphics.shutdown();
	return 0;
}
