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

unsigned int texdata[64] = {
	0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF,
	0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFFFFFFFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF, 0xFF1A1AFF,
};

static const std::string vs = "\n"
"attribute vec3 pos;\n"
"attribute vec2 texcoord;\n"
"attribute vec3 normal;\n"
"uniform vec2 scale;\n"
"varying vec2 texcoord0;\n"
"void main()\n"
"{\n"
"	texcoord0 = texcoord;\n"
"	gl_Position = vec4(pos * vec3(scale, 1.0), 1.0);\n"
"}\n";
static const std::string fs = "\n"
"uniform sampler2D tex;\n"
"varying vec2 texcoord0;\n"
"void main()\n"
"{\n"
"	gl_FragColor = texture2D(tex, texcoord0);\n"
"}\n";

int main(int argc, char **argv)
{
	// Initialize CoreRender
	cr::render::GraphicsEngine graphics;
	if (!graphics.init(cr::render::VideoDriverType::OpenGL, 800, 600, false, 0, true))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(cr::core::LogLevel::Debug);
	cr::res::ResourceManager *rmgr = graphics.getResourceManager();

	// Load some resources
	//cr::render::Texture2D::Ptr texture = graphics.loadTexture2D("test.png");
	cr::render::Texture2D::Ptr texture = graphics.createTexture2D();
	texture->set(8,
	             8,
	             cr::render::TextureFormat::RGBA8,
	             cr::render::TextureFormat::RGBA8,
	             texdata);
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
	shader->addTexture("tex");
	float defscale[2] = {1.0f, 1.0f};
	shader->addUniform("scale", cr::render::ShaderVariableType::Float2, defscale);
	cr::render::Material::Ptr material = new cr::render::Material(rmgr,
	                                                              "testmat");
	material->addTexture("tex", texture);
	material->setShader(shader);
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
	job->uniforms = shader->getUniformData();
	cr::math::Vector2F scale(0.5f, 0.5f);
	job->uniforms["scale"] = scale;
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
		cr::render::InputEvent input;
		while (graphics.getInput(&input))
		{
			switch (input.type)
			{
				case cr::render::InputEventType::WindowClosed:
					stopping = true;
					break;
				case cr::render::InputEventType::KeyDown:
					if (input.keyboard.key == cr::render::KeyCode::Up)
						scale.y *= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Down)
						scale.y /= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Left)
						scale.x *= 1.1f;
					if (input.keyboard.key == cr::render::KeyCode::Right)
						scale.x /= 1.1f;
					job->uniforms["scale"] = scale;
					break;
				default:
					break;
			}
		}
		// Begin frame
		graphics.beginFrame();
		// Render objects
		pipeline->submit(job);
		// Finish and render frame
		graphics.endFrame();
		
	}

	// Delete resources
	delete job;
	texture = 0;
	vb = 0;
	ib = 0;
	shader = 0;
	material = 0;
	pipeline = 0;
	pass = 0;
	// TODO: Other resources

	graphics.shutdown();
	return 0;
}