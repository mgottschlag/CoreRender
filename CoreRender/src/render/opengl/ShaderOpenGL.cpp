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
#include "CoreRender/render/Renderer.hpp"

#include <GL/glew.h>
#include <sstream>

namespace cr
{
namespace render
{
namespace opengl
{
	ShaderOpenGL::ShaderOpenGL(Renderer *renderer,
	                           res::ResourceManager *rmgr,
	                           const std::string &name)
		: Shader(renderer, rmgr, name)
	{
	}
	ShaderOpenGL::~ShaderOpenGL()
	{
	}

	bool ShaderOpenGL::create()
	{
		// Nothing to do here, the real work gets done in uploadShader()
		return true;
	}
	bool ShaderOpenGL::destroy()
	{
		// Delete current shader
		if (handle != 0)
		{
			for (unsigned int i = 0; i < shaders.size(); i++)
			{
				glDetachShader(handle, shaders[i]);
				glDeleteShader(shaders[i]);
			}
			glDeleteProgram(handle);
			shaders.clear();
			handle = 0;
		}
		// Delete old shader
		if (oldhandle != 0)
		{
			for (unsigned int i = 0; i < oldshaders.size(); i++)
			{
				glDetachShader(oldhandle, oldshaders[i]);
				glDeleteShader(oldshaders[i]);
			}
			glDeleteProgram(oldhandle);
			oldshaders.clear();
			oldhandle = 0;
		}
		return true;
	}
	bool ShaderOpenGL::upload()
	{
		// Delete the old shader as it is not in use any longer
		if (oldhandle != 0)
		{
			for (unsigned int i = 0; i < oldshaders.size(); i++)
			{
				glDetachShader(oldhandle, oldshaders[i]);
				glDeleteShader(oldshaders[i]);
			}
			glDeleteProgram(oldhandle);
			oldshaders.clear();
			oldhandle = 0;
		}
		return true;
	}
	void ShaderOpenGL::uploadShader()
	{
		// Make sure no error happened before
		int error = glGetError();
		if (error != GL_NO_ERROR)
		{
			getRenderer()->getLog()->error("Error before uploadShader(): %s",
			                               gluErrorString(error));
		}
		// Move the current shader
		// We cannot delete it as it might be in use right now
		// TODO
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
			getRenderer()->getLog()->error("Could not create shader objects: %s",
			                               gluErrorString(error));
			return;
		}
		glCompileShader(vshader);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getRenderer()->getLog()->error("Could not compile vertex shader: %s",
			                               gluErrorString(error));
			return;
		}
		int status;
		glGetShaderiv(vshader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getRenderer()->getLog()->error("Could not compile vertex shader.");
			printShaderInfoLog(vshader);
			return;
		}
		printShaderInfoLog(vshader);
		glCompileShader(fshader);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getRenderer()->getLog()->error("Could not compile vertex shader: %s",
			                               gluErrorString(error));
			return;
		}
		glGetShaderiv(fshader, GL_COMPILE_STATUS, &status);
		if (status != GL_TRUE)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			getRenderer()->getLog()->error("Could not compile vertex shader.");
			printShaderInfoLog(fshader);
			return;
		}
		printShaderInfoLog(fshader);
		// Create new program
		handle = glCreateProgram();
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			handle = 0;
			getRenderer()->getLog()->error("Could not create program: %s",
			                               gluErrorString(error));
			return;
		}
		// Attach shaders and link
		glAttachShader(handle, vshader);
		glAttachShader(handle, fshader);
		glLinkProgram(handle);
		error = glGetError();
		if (error != GL_NO_ERROR)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			glDeleteProgram(handle);
			handle = 0;
			getRenderer()->getLog()->error("Could not link program: %s",
			                               gluErrorString(error));
			return;
		}
		glGetProgramiv(handle, GL_LINK_STATUS, &status);
		if (status != GL_TRUE)
		{
			glDeleteShader(vshader);
			glDeleteShader(fshader);
			glDeleteProgram(handle);
			handle = 0;
			getRenderer()->getLog()->error("Could not link program.");
			printShaderInfoLog(fshader);
			return;
		}
		printProgramInfoLog(handle);
		// Get attrib locations
		for (std::map<std::string, int>::iterator it = attribs.begin();
		     it != attribs.end(); it++)
		{
			it->second = glGetAttribLocation(handle, it->first.c_str());
		}
		// Get uniform locations
		for (std::map<std::string, int>::iterator it = uniforms.begin();
		     it != uniforms.end(); it++)
		{
			it->second = glGetUniformLocation(handle, it->first.c_str());
		}
		// Get texture locations
		for (std::map<std::string, int>::iterator it = textures.begin();
		     it != textures.end(); it++)
		{
			it->second = glGetUniformLocation(handle, it->first.c_str());
		}
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
			core::Log::Ptr log = getRenderer()->getLog();
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
			core::Log::Ptr log = getRenderer()->getLog();
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
