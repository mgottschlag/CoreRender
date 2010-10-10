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
#include "CoreScene.hpp"

using namespace cr;

int main(int argc, char **argv)
{
	render::GraphicsEngine graphics;
	// Initialize file system
	{
		core::StandardFileSystem::Ptr filesystem;
		filesystem = new core::StandardFileSystem();
		filesystem->mount("Tutorials/media/", "/", core::FileAccess::Read);
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
	// Create scene
	scene::Scene *scene = new scene::Scene(&graphics);
	// Fill scene
	scene::SceneNode::Ptr rotationnode = scene->addGroupNode();
	{
		scene::ModelSceneNode::Ptr model;
		model = scene->addModelNode("/models/jeep.model.xml");
		model->setPosition(math::Vector3F(5.0, 0.0, 0.0));
		model = scene->addModelNode("/models/dwarf.model.xml");
		model->setPosition(math::Vector3F(-5.0, 0.0, 0.0));
		model->setScale(math::Vector3F(0.1, 0.1, 0.1));
		model = scene->addModelNode("/models/plane.model.xml");
		model->setScale(math::Vector3F(10.0, 1.0, 10.0));
		scene::PointLightSceneNode::Ptr pointlight;
		pointlight = scene->addPointLightNode(0, "POINTLIGHT", "");
		pointlight->setPosition(math::Vector3F(0, 3, 0));
		pointlight->setRadius(10.0f);
		pointlight->setColor(core::Color(255, 0, 0, 255));
		scene::SpotLightSceneNode::Ptr spotlight;
		spotlight = scene->addSpotLightNode(0, "SPOTLIGHT", "", rotationnode);
		spotlight->setPosition(math::Vector3F(0, 10, -10));
		spotlight->setRotation(math::Vector3F(45.0, 0.0, 0.0));
		spotlight->setRadius(25.0f);
		spotlight->setColor(core::Color(0, 255, 0, 255));
		scene::CameraSceneNode::Ptr camera;
		camera = scene->addCameraNodePerspective("/pipelines/Forward.pipeline.xml",
		                                         90.0f, 4.0f/3.0f, 1.0f, 1000.0f);
		camera->setPosition(math::Vector3F(0.0, 10.0, 10.0));
		camera->setRotation(math::Vector3F(-45.0, 0.0, 0.0));
		scene->activateCamera(camera, 0);
	}
	// Render loop
	graphics.getLog()->info("Starting rendering.");
	bool stopping = false;
	float rotation = 0.0f;
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
		// Rotate light
		rotation += 0.5f;
		rotationnode->setRotation(math::Vector3F(0, rotation, 0));
		// Begin frame
		graphics.beginFrame();
		// Render scene
		scene->render();
		// Finish and render frame
		graphics.endFrame();
	}

	// Destroy scene again
	delete scene;
	graphics.shutdown();
	return 0;
}
