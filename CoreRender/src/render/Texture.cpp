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

#include "CoreRender/render/Texture.hpp"
#include "CoreRender/render/Renderer.hpp"

namespace cr
{
namespace render
{
	bool TextureFormat::supported(RenderCaps *caps,
	                              TextureFormat::List internalformat,
	                              TextureFormat::List format)
	{
		// TODO
		return false;
	}

	unsigned int TextureFormat::getSize(TextureFormat::List format,
	                                     unsigned int texels)
	{
		switch (format)
		{
			case TextureFormat::Invalid:
				return 0;
			case TextureFormat::RGBA8:
				return texels * 4;
			case TextureFormat::RGBA16F:
				return texels * 8;
			case TextureFormat::RGBA32F:
				return texels * 16;
			case TextureFormat::RGBA_DXT1:
				return texels / 2;
			case TextureFormat::RGBA_DXT3:
			case TextureFormat::RGBA_DXT5:
				return texels;
			case TextureFormat::Depth24Stencil8:
				return texels * 4;
			case TextureFormat::Depth16:
				return texels * 2;
			case TextureFormat::Depth24:
				return texels * 3;
			default:
				// TODO
				return 0;
		}
	}

	Texture::Texture(Renderer *renderer,
	                 res::ResourceManager *rmgr,
	                 const std::string &name,
	                 TextureType::List type)
		: RenderResource(renderer, rmgr, name), handle(0), type(type)
	{
	}
	Texture::~Texture()
	{
	}

}
}
