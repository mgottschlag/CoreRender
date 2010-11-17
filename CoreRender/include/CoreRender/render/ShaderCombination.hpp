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

#ifndef _CORERENDER_RENDER_SHADERCOMBINATION_HPP_INCLUDED_
#define _CORERENDER_RENDER_SHADERCOMBINATION_HPP_INCLUDED_

#include "RenderObject.hpp"
#include "BlendMode.hpp"
#include "DepthTest.hpp"

#include <vector>
#include <string>

namespace cr
{
namespace render
{
	class Shader;

	struct UniformLocations
	{
		int worldmat;
		int worldnormalmat;
		int viewmat;
		int viewmatinv;
		int projmat;
		int viewprojmat;
		int skinmat;
		int viewerpos;
		int framebufsize;
		int lightpos;
		int lightdir;
		int lightcolor;
		int shadowmat;
		int shadowbias;
		int shadowsplitdist;
		int shadowmap;
	};
	struct ShaderCombination : public RenderObject
	{
		ShaderCombination(UploadManager &uploadmgr)
			: RenderObject(uploadmgr)
		{
			programobject = 0;
			shaderobjects[0] = 0;
			shaderobjects[1] = 0;
			shaderobjects[2] = 0;
			shaderobjects[3] = 0;
		}
		virtual ~ShaderCombination();

		struct ShaderCombinationData
		{
			std::string vs;
			std::string fs;
			std::string gs;
			std::string ts;

			BlendMode::List blendmode;
			bool depthwrite;
			DepthTest::List depthtest;
		};

		virtual void upload(void *data);
		virtual void *getUploadData();

		unsigned int compilerflags;
		bool instancing;
		bool skinning;
		ShaderCombinationData currentdata;
		ShaderCombinationData uploadeddata;

		UniformLocations uniforms;
		std::vector<int> customuniforms;
		std::vector<int> attriblocations;
		std::vector<int> samplerlocations;
		/**
		 * Location of the transMat attribute for instancing.
		 */
		int transmatattrib;

		Shader *shader;

		unsigned int programobject;
		unsigned int shaderobjects[4];

		typedef core::SharedPointer<ShaderCombination> Ptr;
	};
}
}

#endif
