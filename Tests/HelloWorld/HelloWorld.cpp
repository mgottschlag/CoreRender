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

static const std::string vs = "\n"
"attribute vec3 pos;\n"
"attribute vec2 texcoord;\n"
"attribute vec3 normal;\n"
"void main()\n"
"{\n"
"	gl_Position = vec4(pos, 1.0);\n"
"}\n";
static const std::string fs = "\n"
"void main()\n"
"{\n"
"	gl_FragColor = vec4(1.0, 1.0, 0.0, 1.0);\n"
"}\n";

int main(int argc, char **argv)
{
	// Initialize CoreRender
	cr::render::GraphicsEngine graphics;
	if (!graphics.init(cr::render::VideoDriverType::OpenGL, 800, 600, false, 0, false))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(cr::core::LogLevel::Debug);
	cr::res::ResourceManager *rmgr = graphics.getResourceManager();

	// Load some resources
	cr::render::Texture2D::Ptr texture = graphics.loadTexture2D("test.png");
	cr::render::VertexBuffer::Ptr vb = graphics.createVertexBuffer();
	vb->set(24 * 8 * sizeof(float), vertices);
	cr::render::IndexBuffer::Ptr ib = graphics.createIndexBuffer();
	ib->set(36 * sizeof(unsigned short), indices);
	cr::render::ShaderText::Ptr shader = graphics.createShaderText("testshader");
	shader->addText("VS_COMMON", vs);
	shader->addText("FS_AMBIENT", fs);
	shader->addContext("AMBIENT", "VS_COMMON", "FS_AMBIENT");
	shader->addAttrib("pos");
	shader->addAttrib("texcoord");
	shader->addAttrib("normal");
	cr::render::Material::Ptr material = new cr::render::Material(rmgr,
	                                                              "testmat");
	material->addTexture("tex", texture);
	material->setShader(shader);
	// Create render job
	cr::render::RenderJob *job = new cr::render::RenderJob;
	job->vertices = vb;
	job->indices = ib;
	job->endindex = 36;
	job->material = material;
	// TODO: Attribs, uniforms
	// Setup pipeline
	cr::render::Pipeline::Ptr pipeline = new cr::render::Pipeline();
	cr::render::RenderPass::Ptr pass = new cr::render::RenderPass("AMBIENT");
	pipeline->addPass(pass);
	graphics.addPipeline(pipeline);
	// Wait for resources to be loaded
	texture->waitForLoading(false);
	// Render loop
	bool stopping = false;
	while (!stopping)
	{
		// Process input
		// TODO
		// Begin frame
		graphics.beginFrame();
		// Render objects
		pipeline->submit(job);
		// Finish and render frame
		graphics.endFrame();
	}

	// Delete resources
	delete job;
	// TODO: Other resources

	graphics.shutdown();
	return 0;
}