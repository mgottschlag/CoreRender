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
	if (!graphics.init(cr::render::VideoDriverType::OpenGL, 800, 600, false, 0, false))
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
	cr::render::Model::Ptr cube = rmgr->getOrLoad<cr::render::Model>("Model",
	                                                                 "/models/cube.model.xml");
	// Create render target
	cr::render::Texture2D::Ptr targettex = rmgr->createResource<cr::render::Texture2D>("Texture2D");
	cr::render::RenderTarget::Ptr target;
	target = rmgr->createResource<cr::render::RenderTarget>("RenderTarget");
	{
		const unsigned int targetsize = 512;
		unsigned int texdata[targetsize * targetsize];
		for (unsigned int i = 0; i < targetsize * targetsize; i++)
			texdata[i] = 0xFF222222;
		targettex->set(targetsize, targetsize,
		               cr::render::TextureFormat::RGBA8,
		               cr::render::TextureFormat::RGBA8,
		               texdata);
		cr::render::FrameBuffer::Ptr fb = rmgr->createResource<cr::render::FrameBuffer>("FrameBuffer");
		fb->setSize(targetsize, targetsize, true);
		target->setFrameBuffer(fb);
		target->addColorBuffer(targettex);
	}
	// Setup first pipeline (renders the model to the texture)
	cr::render::Pipeline::Ptr rttpipeline = new cr::render::Pipeline();
	cr::render::PipelineSequence *rttsequence = rttpipeline->addSequence("main");
	rttpipeline->setDefaultSequence(rttsequence);
	cr::render::PipelineStage *stage = rttsequence->addStage("scene");
	cr::render::SetTargetCommand *settarget = new cr::render::SetTargetCommand();
	settarget->setTarget(target);
	stage->addCommand(settarget);
	cr::render::ClearCommand *clear = new cr::render::ClearCommand();
	clear->clearDepth(true, 1.0f);
	clear->clearColor(0, true, cr::core::Color(255, 255, 255, 255));
	stage->addCommand(clear);
	cr::render::BatchListCommand *batchlist = new cr::render::BatchListCommand();
	batchlist->setContext("AMBIENT");
	stage->addCommand(batchlist);
	graphics.addPipeline(rttpipeline);
	rttsequence->getDefaultUniforms().push_back(cr::render::DefaultUniform(cr::render::DefaultUniformName::ProjMatrix,
	                                                                       cr::math::Matrix4::Ortho(100.0f, 100.0f, 100.0f, -100.0f)));
	// Setup second pipeline (renders the cube onto the screen)
	cr::render::Pipeline::Ptr pipeline = new cr::render::Pipeline();
	cr::render::PipelineSequence *sequence = pipeline->addSequence("main");
	pipeline->setDefaultSequence(sequence);
	stage = sequence->addStage("scene");
	clear = new cr::render::ClearCommand();
	clear->clearDepth(true, 1.0f);
	clear->clearColor(0, true, cr::core::Color(60, 60, 60, 255));
	stage->addCommand(clear);
	batchlist = new cr::render::BatchListCommand();
	batchlist->setContext("AMBIENT");
	stage->addCommand(batchlist);
	graphics.addPipeline(pipeline);
	sequence->getDefaultUniforms().push_back(cr::render::DefaultUniform(cr::render::DefaultUniformName::ProjMatrix,
	                                                                    cr::math::Matrix4::Ortho(4.0f, 4.0f, 4.0f, -4.0f)));
	// Get material
	// We need to change the texture of the material later
	cr::render::Material::Ptr material;
	material = rmgr->getOrLoad<cr::render::Material>("Material", "/models/cube.material.xml");
	material->addTexture("tex", targettex);
	// Wait for resources to be loaded
	model->waitForLoading(true);
	anim->waitForLoading(true);
	cube->waitForLoading(true);
	material->waitForLoading(true);
	// Set the texture of the cube material to our render target texture
	material->addTexture("tex", targettex);
	// Create renderable
	cr::render::ModelRenderable *renderable = new cr::render::ModelRenderable();
	renderable->setModel(model);
	renderable->setTransMat(cr::math::Matrix4::TransMat(cr::math::Vector3F(0.0f, -40.0f, 0.0f)));
	renderable->addAnimStage(anim, 1.0);
	// Create cube renderable
	cr::render::ModelRenderable *cuberenderable = new cr::render::ModelRenderable();
	cuberenderable->setModel(cube);
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
		// Render model to texture
		animtime += 0.001f;
		if (animtime >= anim->getFrameCount() / anim->getFramesPerSecond())
			animtime -= anim->getFrameCount() / anim->getFramesPerSecond();
		renderable->getAnimStage(0)->time = animtime;
		rttsequence->submit(renderable);
		// Draw cube
		rotation += 0.2f;
		cr::math::Quaternion quat(cr::math::Vector3F(rotation, rotation * 0.3f, rotation * 0.5f));
		cuberenderable->setTransMat(quat.toMatrix());
		sequence->submit(cuberenderable);
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
	delete cuberenderable;
	target = 0;
	rttpipeline = 0;
	pipeline = 0;
	model = 0;
	anim = 0;
	cube = 0;
	material = 0;
	targettex = 0;

	graphics.shutdown();
	return 0;
}
