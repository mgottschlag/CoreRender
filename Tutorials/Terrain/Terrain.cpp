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

#include <GL/glfw.h>

using namespace cr;

math::Vector3F camerarot(-40, 0, 0);

void mouseMovementListener(int x, int y)
{
	camerarot = math::Vector3F(-y, -x, 0) * 0.1;
}

int main(int argc, char **argv)
{
	// Create render window
	glfwInit();
	if (!glfwOpenWindow(1024,768, 8, 8, 8, 8, 24, 8, GLFW_WINDOW))
	{
		std::cerr << "Failed to open render window!" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwSetMousePosCallback(mouseMovementListener);
	glfwEnable(GLFW_STICKY_KEYS);
	glfwDisable(GLFW_MOUSE_CURSOR);
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
	res::ResourceManager *rmgr = graphics.getResourceManager();
	// Create scene
	scene::Scene scene(rmgr);
	// Add the terrain
	core::File::Ptr terrainfile = rmgr->getFileSystem()->open("/Terrain.data",
	                                                          core::FileAccess::Read,
	                                                          false);
	float *terraindata = new float[257 * 257];
	terrainfile->read(257 * 257 * sizeof(float), terraindata);
	scene::Terrain::Ptr terrain = rmgr->createResource<scene::Terrain>("Terrain", "testterrain");
	terrain->set(257, 257, 65, terraindata);
	delete[] terraindata;
	terrainfile = 0;
	render::Material::Ptr terrainmat = graphics.getMaterial("/materials/Terraintest.material.xml");
	terrainmat->waitForLoading(true);
	terrain->setMaterial(terrainmat);
	// Add a camera
	scene::Camera::Ptr camera = new scene::Camera;
	camera->setProjMat(math::Matrix4::PerspectiveFOV(90.0f, 4.0f/3.0f, 1.0f, 1000.0f));
	math::Matrix4 viewmat = math::Matrix4::TransMat(0.0f, 0.0f, -300.0f)
	                      * math::Quaternion(math::Vector3F(30.0f, 0.0f, 0.0f)).toMatrix();
	camera->setViewMat(viewmat);
	// Setup render pipeline
	{
		render::Pipeline::Ptr pipeline;
		pipeline = graphics.getPipeline("/pipelines/Forward.pipeline.xml");
		camera->setPipeline(pipeline);
		camera->setViewport(0, 0, 1024, 768);
		pipeline->waitForLoading(false);
		pipeline->resizeTargets(1024, 768);
	}
	// Add the camera and the lights to the scene
	scene.addCamera(camera);
	// Render loop
	bool stopping = false;
	core::Time fpstime = core::Time::Now();
	int fps = 0;
	math::Vector3F camerapos(0, 100, 0);
	while (!stopping)
	{
		// Process input
		bool fast = false;
		if (glfwGetKey(GLFW_KEY_LSHIFT) == GLFW_PRESS)
			fast = true;
		math::Vector3F movement(0, 0, 0);
		if (glfwGetKey('W') == GLFW_PRESS)
			movement.z -= 1;
		if (glfwGetKey('S') == GLFW_PRESS)
			movement.z += 1;
		if (glfwGetKey('A') == GLFW_PRESS)
			movement.x -= 1;
		if (glfwGetKey('D') == GLFW_PRESS)
			movement.x += 1;
		movement = movement * 0.1;
		if (fast)
			movement = movement * 10;
		math::Matrix4 rotationmat = math::Quaternion(camerarot).toMatrix();
		movement = rotationmat.transformPoint(movement);
		camerapos += movement;
		math::Matrix4 viewmatinv = math::Matrix4::TransMat(camerapos) * rotationmat;
		viewmat = viewmatinv.inverse();
		// Set animations
		camera->setViewMat(viewmat);
		// Render objects
		render::FrameData *frame = graphics.beginFrame();
		render::SceneFrameData *scenedata = scene.beginFrame(frame);
		render::RenderQueue *renderqueues = scenedata->getRenderQueues();
		unsigned int queuecount = scenedata->getRenderQueueCount();
		for (unsigned int i = 0; i < queuecount; i++)
		{
			//terrain->render(renderqueues[i], math::Matrix4::ScaleMat(300.0f, 1.0f, 300.0f), cameraPos);
			//terrain->render(renderqueues[i], math::Matrix4::TransMat(-512.0f, 0.0f, -512.0f) * math::Matrix4::ScaleMat(1024.0f, 1.0f, 1024.0f), math::Vector3F(100, 100, 200));
			terrain->render(renderqueues[i], math::Matrix4::TransMat(-512.0f, 0.0f, -512.0f) * math::Matrix4::ScaleMat(1024.0f, 100.0f, 1024.0f), camerapos);
		}
		graphics.endFrame(frame);
		// Render frame
		graphics.render(frame);
		// Swap buffers
		glfwSwapBuffers();
		if (glfwGetWindowParam(GLFW_OPENED) == GL_FALSE || glfwGetKey(GLFW_KEY_ESC) == GLFW_PRESS)
			stopping = true;
		fps++;
		core::Time currenttime = core::Time::Now();
		if (currenttime - fpstime >= core::Duration::Seconds(1))
		{
			std::cout << "FPS: " << fps << std::endl;
			fpstime = currenttime;
			fps = 0;
		}
	}
	// Destroy ressources
	scene.destroy();
	camera = 0;
	terrain = 0;
	terrainmat = 0;
	graphics.shutdown();
	// Close render window
	glfwTerminate();
	return 0;
}
