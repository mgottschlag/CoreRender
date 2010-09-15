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

float vertices[4 * 4] = {
	-1.0f, -1.0f, 0.0f, 0.0f,
	1.0f, -1.0f, 1.0f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 0.0f, 1.0f
};

unsigned char indices[6] = {
	0, 1, 2,
	0, 2, 3
};

const unsigned int targetwidth = 400;
const unsigned int targetheight = 300;

cr::render::RenderJob *createFSQuadJob(cr::render::VertexBuffer::Ptr vb,
                                       cr::render::IndexBuffer::Ptr ib,
                                       cr::render::Material::Ptr material,
                                       cr::render::VertexLayout::Ptr layout)
{
	cr::render::RenderJob *job = new cr::render::RenderJob;
	job->vertices = vb;
	job->vertexcount = 4;
	job->indices = ib;
	job->endindex = 6;
	job->indextype = 1;
	job->startindex = 0;
	job->basevertex = 0;
	job->vertexoffset = 0;
	job->material = material;
	job->layout = layout;
	job->uniforms.add("targetsize") = cr::math::Vector2F(targetwidth, targetheight);
	return job;
}

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
	cr::render::Model::Ptr cube = rmgr->getOrLoad<cr::render::Model>("Model",
	                                                                 "/models/cube.model.xml");
	// Create a single frame buffer for all passes
	// This can be done as all use a similar setup and have the same size and is
	// much faster than using multiple frame buffers
	cr::render::FrameBuffer::Ptr fb = rmgr->createResource<cr::render::FrameBuffer>("FrameBuffer");
	fb->setSize(targetwidth, targetheight, false);
	cr::render::FrameBuffer::Ptr scenefb = rmgr->createResource<cr::render::FrameBuffer>("FrameBuffer");
	scenefb->setSize(targetwidth, targetheight, true);
	// Create two textures with half the size of the screen
	// We will use one of them as the target and one as the source texture and
	// swap in every pass
	cr::render::Texture2D::Ptr targettex1 = rmgr->createResource<cr::render::Texture2D>("Texture2D");
	targettex1->set(targetwidth, targetheight, cr::render::TextureFormat::RGBA8);
	cr::render::Texture2D::Ptr targettex2 = rmgr->createResource<cr::render::Texture2D>("Texture2D");
	targettex2->set(targetwidth, targetheight, cr::render::TextureFormat::RGBA8);
	// Create first render target to render the scene to a texture
	cr::render::RenderTarget::Ptr scenetarget;
	scenetarget = rmgr->createResource<cr::render::RenderTarget>("RenderTarget");
	scenetarget->setFrameBuffer(scenefb);
	scenetarget->addColorBuffer(targettex1);
	// Create second render target (horizontal blur pass)
	cr::render::RenderTarget::Ptr blurtarget1;
	blurtarget1 = rmgr->createResource<cr::render::RenderTarget>("RenderTarget");
	blurtarget1->setFrameBuffer(fb);
	blurtarget1->addColorBuffer(targettex2);
	// Create second render target (vertical blur pass)
	cr::render::RenderTarget::Ptr blurtarget2;
	blurtarget2 = rmgr->createResource<cr::render::RenderTarget>("RenderTarget");
	blurtarget2->setFrameBuffer(fb);
	blurtarget2->addColorBuffer(targettex1);
	// Setup first pipeline (renders the model to the texture)
	cr::render::Pipeline::Ptr scenepipeline = new cr::render::Pipeline();
	{
		cr::render::RenderPass::Ptr scenepass = new cr::render::RenderPass("AMBIENT");
		scenepass->setClear(true, true, cr::core::Color(60, 60, 60, 255));
		scenepass->setRenderTarget(scenetarget);
		scenepipeline->addPass(scenepass);
	}
	graphics.addPipeline(scenepipeline);
	// Setup second pipeline (blurs the texture and renders it to the screen)
	cr::render::Pipeline::Ptr blurpipeline = new cr::render::Pipeline();
	{
		// Vertical blur pass
		cr::render::RenderPass::Ptr verticalpass = new cr::render::RenderPass("VERTICAL_BLUR");
		// TODO: Disable Z culling
		//verticalpass->setClear(false, false);
		verticalpass->setClear(true, true, cr::core::Color(255, 60, 60, 255));
		verticalpass->setRenderTarget(blurtarget1);
		blurpipeline->addPass(verticalpass);
		// Horizontal blur pass
		cr::render::RenderPass::Ptr horizontalpass = new cr::render::RenderPass("HORIZONTAL_BLUR");
		// TODO: Disable Z culling
		//horizontalpass->setClear(false, false);
		horizontalpass->setClear(true, true, cr::core::Color(60, 255, 60, 255));
		horizontalpass->setRenderTarget(blurtarget2);
		blurpipeline->addPass(horizontalpass);
		// Draw the texture to the screen
		cr::render::RenderPass::Ptr screenpass = new cr::render::RenderPass("COPY_COLOR");
		// TODO: Disable Z culling
		//screenpass->setClear(false, false);
		screenpass->setClear(true, true, cr::core::Color(60, 60, 255, 255));
		blurpipeline->addPass(screenpass);
	}
	graphics.addPipeline(blurpipeline);
	// Load shaders
	cr::render::ShaderText::Ptr blurshader1;
	blurshader1 = rmgr->getOrLoad<cr::render::ShaderText>("ShaderText",
	                                                      "/shaders/vblur.shader.xml");
	cr::render::ShaderText::Ptr blurshader2;
	blurshader2 = rmgr->getOrLoad<cr::render::ShaderText>("ShaderText",
	                                                      "/shaders/hblur.shader.xml");
	cr::render::ShaderText::Ptr copyshader;
	copyshader = rmgr->getOrLoad<cr::render::ShaderText>("ShaderText",
	                                                     "/shaders/copy.shader.xml");
	// Create material for horizontal blur pass
	cr::render::Material::Ptr blurmaterial1 = rmgr->createResource<cr::render::Material>("Material");
	blurmaterial1->addTexture("source", targettex1);
	blurmaterial1->setShader(blurshader1);
	// Create material for vertical blur pass
	cr::render::Material::Ptr blurmaterial2 = rmgr->createResource<cr::render::Material>("Material");
	blurmaterial2->addTexture("source", targettex2);
	blurmaterial2->setShader(blurshader2);
	// Create material for final screen pass
	cr::render::Material::Ptr copymaterial = rmgr->createResource<cr::render::Material>("Material");
	copymaterial->addTexture("source", targettex1);
	copymaterial->setShader(copyshader);
	// Create fullscreen quad for blur/screen passes
	cr::render::VertexBuffer::Ptr fsquadvb = rmgr->createResource<cr::render::VertexBuffer>("VertexBuffer");
	fsquadvb->set(4 * 4 * sizeof(float), vertices);
	cr::render::IndexBuffer::Ptr fsquadib = rmgr->createResource<cr::render::IndexBuffer>("IndexBuffer");
	fsquadib->set(6 * sizeof(unsigned char), indices);
	// Create vertex layout
	cr::render::VertexLayout::Ptr layout = new cr::render::VertexLayout(2);
	layout->setElement(0, "pos", 0, 2, 0, cr::render::VertexElementType::Float, 16);
	layout->setElement(1, "texcoord0", 0, 2, 8, cr::render::VertexElementType::Float, 16);
	// Create render job for the horizontal blur pass
	cr::render::RenderJob *blurjob1 = createFSQuadJob(fsquadvb, fsquadib, blurmaterial1, layout);
	// Create render job for the vertical blur pass
	cr::render::RenderJob *blurjob2 = createFSQuadJob(fsquadvb, fsquadib, blurmaterial2, layout);
	// Create render job for the final screen pass
	cr::render::RenderJob *screenjob = createFSQuadJob(fsquadvb, fsquadib, copymaterial, layout);
	// Setup camera matrix for the scene
	/*cr::math::Matrix4 projmat = cr::math::Matrix4::PerspectiveFOV(60.0f, 4.0f/3.0f, 1.0f, 1000.0f);
	projmat = projmat * cr::math::Matrix4::TransMat(cr::math::Vector3F(0, 0, -100));
	projmat = projmat * cr::math::Quaternion(cr::math::Vector3F(45.0, 0.0, 0.0)).toMatrix();*/
	cr::math::Matrix4 projmat = cr::math::Matrix4::Ortho(100.0f, 100.0f, 100.0f, -100.0f);
	projmat = projmat * cr::math::Matrix4::TransMat(cr::math::Vector3F(0, -40, 0));
	// Create renderable
	cr::render::ModelRenderable *renderable = new cr::render::ModelRenderable();
	renderable->setModel(model);
	renderable->setProjMat(projmat);
	renderable->setTransMat(cr::math::Matrix4::TransMat(cr::math::Vector3F(0.0f, -40.0f, 0.0f)));
	renderable->addAnimStage(anim, 1.0);
	// Wait for resources to be loaded
	model->waitForLoading(true);
	anim->waitForLoading(true);
	blurshader1->waitForLoading(true);
	blurshader2->waitForLoading(true);
	copyshader->waitForLoading(true);
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
		// Render scene to texture
		animtime += 0.001f;
		if (animtime >= anim->getFrameCount() / anim->getFramesPerSecond())
			animtime -= anim->getFrameCount() / anim->getFramesPerSecond();
		renderable->getAnimStage(0)->time = animtime;
		rotation += 0.1f;
		cr::math::Quaternion quat(cr::math::Vector3F(0, rotation, 0));
		renderable->setTransMat(quat.toMatrix());
		scenepipeline->submit(renderable);
		// Draw blur passes
		blurpipeline->submit(blurjob1);
		blurpipeline->submit(blurjob2);
		blurpipeline->submit(screenjob);
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
			std::cout << "Time: " << stats.getRenderTime().getNanoseconds()
				<< " rendering, " << stats.getWaitTime().getNanoseconds()
				<< " waiting." << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}

	// Delete resources
	delete renderable;
	delete blurjob1;
	delete blurjob2;
	delete screenjob;
	scenetarget = 0;
	blurtarget1 = 0;
	blurtarget2 = 0;
	scenepipeline = 0;
	blurpipeline = 0;
	model = 0;
	anim = 0;
	cube = 0;
	blurmaterial1 = 0;
	blurmaterial2 = 0;
	copymaterial = 0;
	blurshader1 = 0;
	blurshader2 = 0;
	copyshader = 0;
	targettex1 = 0;
	targettex2 = 0;
	fb = 0;
	scenefb = 0;
	layout = 0;
	fsquadvb = 0;
	fsquadib = 0;

	graphics.shutdown();
	return 0;
}
