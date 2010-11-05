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

using namespace cr;

int main(int argc, char **argv)
{
	// Create render window
	// TODO
	// Initialize file system
	GraphicsEngine graphics;
	{
		core::StandardFileSystem::Ptr filesystem;
		filesystem = new core::StandardFileSystem();
		filesystem->mount("Tutorials/media/", "/", core::FileAccess::Read);
		// Writes (e.g. log) shall go into the working directory
		filesystem->mount("", "/", core::FileAccess::Write);
		graphics.setFileSystem(filesystem);
	}
	// Initialize graphics engine
	if (!graphics.init())
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(core::LogLevel::Debug);
	// Create scene
	scene::Scene scene(&graphics);
	// Add some models
	scene::Mesh::Ptr jeep = graphics.getMesh("/models/jeep.mesh.xml");
	scene::Mesh::Ptr dwarf = graphics.getMesh("/models/dwarf.mesh.xml");
	scene::AnimatedMesh::Ptr dwarf2 = new scene::AnimatedMesh(dwarf);
	scene::Animation::Ptr dwarfanim = graphics.getAnimation("/models/dwarf.anim.xml");
	dwarf2->addAnimation(dwarfanim, 1.0f);
	math::Matrix4 dwarf2positions[1024];
	for (unsigned int i = 0; i < 1024; i++)
	{
		float x = (float)(i % 32) * 10.0f;
		float z = (float)(i / 32) * 10.0f;
		dwarf2positions[i] = math::Matrix4::TransMat(x, 0.0f, z);
	}
	// Add a terrain
	// TODO
	// Add some lights
	scene::SpotLight::Ptr spotlight = new scene::SpotLight(0, "SPOTLIGHT", "SHADOWMAP");
	spotlight->setRadius(10.0f);
	spotlight->setColor(core::Color(1.0f, 0.0f, 0.0f, 1.0f));
	scene::PointLight::Ptr pointlight = new scene::PointLight(0, "POINTLIGHT", "");
	spotlight->setRadius(10.0f);
	spotlight->setColor(core::Color(0.0f, 1.0f, 0.0f, 1.0f));
	// Add a camera
	scene::Camera::Ptr camera = new scene::Camera;
	camera->setPipeline(graphics.getPipeline("/pipelines/Forward.pipeline.xml"));
	camera->setViewport(0, 0, 800, 600);
	// Add the camera and the lights to the scene
	scene.addCamera(camera);
	scene.addLight(spotlight);
	scene.addLight(pointlight);
	// Render loop
	bool stopping = false;
	float animtime = 0.0f;
	while (!stopping)
	{
		// Process input
		// TODO
		// Set animations
		animtime = animtime + 0.01f;
		dwarf2->setAnimation(0, animtime);
		// Render objects
		render::FrameData *frame = graphics.beginFrame();
		render::SceneFrameData *scenedata = scene.beginFrame(frame);
		render::RenderQueue *renderqueues = scenedata->getRenderQueues();
		unsigned int queuecount = scenedata->getRenderQueueCount();
		for (unsigned int i = 0; i < queuecount; i++)
		{
			jeep->render(renderqueues[i], math::Matrix4::TransMat(3.0f, 1.0f, 0.0f));
			dwarf->render(renderqueues[i], math::Matrix4::TransMat(3.0f, 1.0f, 0.0f));
			dwarf2->render(renderqueues[i], 1024, dwarf2positions);
		}
		// Render frame
		graphics.render(frame);
		// Swap buffers
		// TODO
	}
	// Close render window
	// TODO
}
