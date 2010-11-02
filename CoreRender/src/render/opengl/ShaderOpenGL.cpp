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

#include "ShaderOpenGL.hpp"
#include "CoreRender/res/ResourceManager.hpp"

#include <GL/glew.h>
#include <sstream>

namespace cr
{
namespace render
{
namespace opengl
{
	ShaderOpenGL::ShaderOpenGL(UploadManager &uploadmgr,
	                           res::ResourceManager *rmgr,
	                           const std::string &name)
		: Shader(uploadmgr, rmgr, name)
	{
	}
	ShaderOpenGL::~ShaderOpenGL()
	{
	}

	void ShaderOpenGL::compileCombination(ShaderCombination *combination)
	{
		std::string vs = combination->uploadedtext.vs;
		std::string fs = combination->uploadedtext.fs;
		std::string gs = combination->uploadedtext.gs;
		std::string ts = combination->uploadedtext.ts;
		// Make sure no error happened before
		int error = glGetError();
		if (error != GL_NO_ERROR)
		{
			getManager()->getLog()->error("Error before uploadShader(): %s",
			                              gluErrorString(error));
		}
		// Delete the old shader
		deleteCombination(combination);
		combination->programobject = 0;
		combination->shaderobjects[0] = 0;
		combination->shaderobjects[1] = 0;
		combination->shaderobjects[2] = 0;
		combination->shaderobjects[3] = 0;
		// Check capabilities
		// TODO
		// Create vertex/fragment shaders
		// TODO: Fix error checking, add descriptive errors
		// TODO: Add geometry/tesselation shaders
		const char *vshadertext = vs.c_str();
		unsigned int vshader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vshader, 1, &vshadertext, NULL);
		const char *fshadertext = fs.c_str();
		unsigned int fshader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fshader, 1, &fshadertext, NULL);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getManager()->getLog()->error("Could not create shader objects: %s",
			                              gluErrorString(error));
			return;
		}
		glCompileShader(vshader);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getManager()->getLog()->error("Could not compile vertex shader: %s",
			                              gluErrorString(error));
			return;
		}
		int status;
		glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			getManager()->getLog()->error("Could not compile vertex shader.");
			printShaderInfoLog(vshader);
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			return;
		}
		printShaderInfoLog(vshader);
		glCompileShader(fshader);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getManager()->getLog()->error("Could not compile fragment shader: %s",
			                              gluErrorString(error));
			return;
		}
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			getManager()->getLog()->error("Could not compile fragment shader.");
			printShaderInfoLog(fshader);
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			return;
		}
		printShaderInfoLog(fshader);
		// Create new program
		unsigned int program = glCreateProgram();
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			program = 0;
			getManager()->getLog()->error("Could not create program: %s",
			                               gluErrorString(error));
			return;
		}
		// Attach shaders and link
		glAttachShader(program, vshader);
		glAttachShader(program, fshader);
		glLinkProgram(program);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			glDeleteProgram(program);
			program = 0;
			getManager()->getLog()->error("Could not link program: %s",
			                               gluErrorString(error));
			return;
		}
		glGetProgramiv(program, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			getManager()->getLog()->error("Could not link program.");
			printShaderInfoLog(fshader);
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			glDeleteProgram(program);
			program = 0;
			return;
		}
		printProgramInfoLog(program);
		// Get attrib locations
		res::NameRegistry &names = getManager()->getNameRegistry();
		combination->attriblocations.resize(uploadedinfo.attribs.size());
		for (unsigned int i = 0; i < uploadedinfo.attribs.size(); i++)
		{
			std::string attribname = names.getAttrib(uploadedinfo.attribs[i]);
			int location = glGetAttribLocation(program, attribname.c_str());
			combination->attriblocations[i] = location;
		}
		// Get default uniform locations
		combination->uniforms.worldmat = glGetUniformLocation(program,
		                                                      "worldMat");
		combination->uniforms.worldnormalmat = glGetUniformLocation(program,
		                                                            "worldNormalMat");
		combination->uniforms.viewmat = glGetUniformLocation(program,
		                                                     "viewMat");
		combination->uniforms.viewmatinv = glGetUniformLocation(program,
		                                                        "viewMatInv");
		combination->uniforms.projmat = glGetUniformLocation(program,
		                                                     "projMat");
		combination->uniforms.viewprojmat = glGetUniformLocation(program,
		                                                         "viewProjMat");
		combination->uniforms.skinmat = glGetUniformLocation(program,
		                                                     "skinMat");
		combination->uniforms.viewerpos = glGetUniformLocation(program,
		                                                       "viewerPos");
		combination->uniforms.framebufsize = glGetUniformLocation(program,
		                                                          "frameBufSize");
		combination->uniforms.lightpos = glGetUniformLocation(program,
		                                                      "lightPos");
		combination->uniforms.lightdir = glGetUniformLocation(program,
		                                                      "lightDir");
		combination->uniforms.lightcolor = glGetUniformLocation(program,
		                                                      "lightColor");
		combination->uniforms.shadowmat = glGetUniformLocation(program,
		                                                      "shadowMat");
		combination->uniforms.shadowbias = glGetUniformLocation(program,
		                                                      "shadowBias");
		combination->uniforms.shadowsplitdist = glGetUniformLocation(program,
		                                                             "shadowSplitDist");
		// Get uniform locations
		combination->customuniforms.resize(uploadedinfo.uniforms.size());
		for (unsigned int i = 0; i < uploadedinfo.uniforms.size(); i++)
		{
			const std::string &name = uploadedinfo.uniforms[i].name;
			combination->customuniforms[i] = glGetUniformLocation(program,
			                                                      name.c_str());
		}
		// Get sampler locations
		combination->samplerlocations.resize(uploadedinfo.samplers.size());
		for (unsigned int i = 0; i < uploadedinfo.samplers.size(); i++)
		{
			const std::string &name = uploadedinfo.samplers[i].name;
			combination->samplerlocations[i] = glGetUniformLocation(program,
			                                                        name.c_str());
		}
		// Store OpenGL objects
		combination->programobject = program;
		combination->shaderobjects[0] = vshader;
		combination->shaderobjects[1] = fshader;
	}
	void ShaderOpenGL::deleteCombination(ShaderCombination *combination)
	{
		for (unsigned int i = 0; i < 4; i++)
		{
			if (combination->shaderobjects[i] != 0)
			{
				glDetachShader(combination->programobject,
				               combination->shaderobjects[i]);
				glDeleteShader(combination->shaderobjects[i]);
			}
		}
		glDeleteProgram(combination->programobject);
	}

	void ShaderOpenGL::printShaderInfoLog(unsigned int shader)
	{
		// Get length
		int length = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			// Get info log
			char *infolog = new char[length];
			int charswritten;
			glGetShaderInfoLog(shader, length, &charswritten, infolog);
			// Split into lines and print to log
			core::Log::Ptr log = getManager()->getLog();
			log->warning("Shader info log:");
			std::string line;
			std::istringstream stream(infolog);
			while (!stream.eof())
			{
				std::getline(stream, line);
				log->warning("%s", line.c_str());
			}
			// Delete again
			delete[] infolog;
		}
	}
	void ShaderOpenGL::printProgramInfoLog(unsigned int program)
	{
		// Get length
		int length = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
		if (length > 1)
		{
			// Get info log
			char *infolog = new char[length];
			int charswritten;
			glGetProgramInfoLog(program, length, &charswritten, infolog);
			// Split into lines and print to log
			core::Log::Ptr log = getManager()->getLog();
			log->warning("Program info log:");
			std::string line;
			std::istringstream stream(infolog);
			while (!stream.eof())
			{
				std::getline(stream, line);
				log->warning("%s", line.c_str());
			}
			// Delete again
			delete[] infolog;
		}
	}
}
}
}
