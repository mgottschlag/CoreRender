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
"uniform mat4 worldMat;\n"
"varying vec2 texcoord0;\n"
"void main()\n"
"{\n"
"	texcoord0 = texcoord;\n"
"	gl_Position = worldMat * vec4(pos * vec3(scale, 1.0), 1.0);\n"
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
	render::GraphicsEngine graphics;
	// Initialize CoreRender
	if (!graphics.init(render::VideoDriverType::OpenGL, 800, 600, false, 0, true))
	{
		std::cerr << "Graphics engine failed to initialize!" << std::endl;
		return -1;
	}
	graphics.getLog()->setConsoleLevel(core::LogLevel::Debug);
	res::ResourceManager *rmgr = graphics.getResourceManager();

	// Load some resources
	render::Texture2D::Ptr texture = rmgr->createResource<render::Texture2D>("Texture2D");
	texture->set(8,
	             8,
	             render::TextureFormat::RGBA8,
	             render::TextureFormat::RGBA8,
	             texdata);
	render::VertexBuffer::Ptr vb = rmgr->createResource<render::VertexBuffer>("VertexBuffer");
	vb->set(24 * 8 * sizeof(float), vertices);
	render::IndexBuffer::Ptr ib = rmgr->createResource<render::IndexBuffer>("IndexBuffer");
	ib->set(36 * sizeof(unsigned short), indices);
	render::ShaderText::Ptr shader = rmgr->createResource<render::ShaderText>("ShaderText");
	shader->addText("VS_COMMON", vs);
	shader->addText("FS_AMBIENT", fs);
	shader->addContext("AMBIENT", "VS_COMMON", "FS_AMBIENT");
	shader->addAttrib("pos");
	shader->addAttrib("texcoord");
	shader->addAttrib("normal");
	shader->addTexture("tex");
	float defscale[2] = {1.0f, 1.0f};
	shader->addUniform("scale", render::ShaderVariableType::Float2, defscale);
	shader->addUniform("worldMat", render::ShaderVariableType::Float4x4, 0);
	render::Material::Ptr material = rmgr->createResource<render::Material>("Material");
	material->addTexture("tex", texture);
	material->setShader(shader);
	// Create vertex layout
	render::VertexLayout::Ptr layout = new render::VertexLayout(3);
	layout->setElement(0, "pos", 0, 3, 0, render::VertexElementType::Float, 32);
	layout->setElement(1, "normal", 0, 3, 12, render::VertexElementType::Float, 32);
	layout->setElement(2, "texcoord", 0, 2, 24, render::VertexElementType::Float, 32);
	// Create render job
	render::RenderJob *job = new render::RenderJob;
	job->vertices = vb;
	job->vertexcount = 24;
	job->indices = ib;
	job->endindex = 36;
	job->indextype = 2;
	job->material = material;
	job->layout = layout;
	job->uniforms = shader->getUniformData();
	math::Vector2F scale(0.5f, 0.5f);
	job->uniforms["scale"] = scale;
	math::Matrix4 matrix = math::Matrix4::ScaleMat(math::Vector3F(0.5f, 0.5f, 0.5f));
	job->defaultuniforms.push_back(render::DefaultUniform(render::DefaultUniformName::TransMatrix, matrix));
	// Setup pipeline
	render::Pipeline::Ptr pipeline = new render::Pipeline();
	render::PipelineSequence *sequence = pipeline->addSequence("main");
	pipeline->setDefaultSequence(sequence);
	render::PipelineStage *stage = sequence->addStage("scene");
	render::ClearCommand *clear = new render::ClearCommand();
	clear->clearDepth(true, 1.0f);
	clear->clearColor(0, true, core::Color(60, 60, 60, 255));
	stage->addCommand(clear);
	render::BatchListCommand *batchlist = new render::BatchListCommand();
	batchlist->setContext("AMBIENT");
	stage->addCommand(batchlist);
	graphics.addPipeline(pipeline);
	sequence->getDefaultUniforms().push_back(render::DefaultUniform(render::DefaultUniformName::ProjMatrix,
	                                                                math::Matrix4::Identity()));
	// Render loop
	bool stopping = false;
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
				case render::InputEventType::KeyDown:
					if (input.keyboard.key == render::KeyCode::Up)
						scale.y *= 1.1f;
					if (input.keyboard.key == render::KeyCode::Down)
						scale.y /= 1.1f;
					if (input.keyboard.key == render::KeyCode::Left)
						scale.x *= 1.1f;
					if (input.keyboard.key == render::KeyCode::Right)
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
		matrix = matrix * math::Quaternion(math::Vector3F(0.0, 0.1, 0.1)).toMatrix();
		job->defaultuniforms[0] = render::DefaultUniform(render::DefaultUniformName::TransMatrix, matrix);
		sequence->submit(job);
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

	graphics.shutdown();
	return 0;
}