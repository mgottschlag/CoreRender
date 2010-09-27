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

#include "CoreRender/render/RenderJob.hpp"
#include "CoreRender/render/Renderer.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "FrameData.hpp"
#include "CoreRender/render/PipelineSequence.hpp"

#include <cstring>

namespace cr
{
namespace render
{
	RenderBatch *RenderJob::createBatch(const std::string &context,
	                                    Renderer *renderer,
	                                    SequenceState *sequence,
	                                    UniformData &uniforms,
	                                    DefaultUniformValues *defuniforms,
	                                    unsigned int flags)
	{
		Shader::Ptr shader = material->getShader()->getShader(context, flags);
		if (!shader)
			return 0;
		// Allocate render batch
		core::MemoryPool *memory = renderer->getNextFrameMemory();
		RenderBatch *batch = (RenderBatch*)memory->allocate(sizeof(RenderBatch));
		batch->startindex = startindex;
		batch->endindex = endindex;
		batch->basevertex = basevertex;
		batch->vertexoffset = vertexoffset;
		batch->indextype = indextype;
		batch->vertices = vertices->getHandle();
		batch->indices = indices->getHandle();
		batch->shader = shader->getHandle();
		batch->blendmode = shader->getBlendMode();
		batch->sortkey = sortkey;
		batch->renderflags = 0;
		// Attribs
		if (layout->getElementCount() > 0)
		{
			batch->attribcount = layout->getElementCount();
			unsigned int memsize = sizeof(AttribMapping) * batch->attribcount;
			batch->attribs = (AttribMapping*)memory->allocate(memsize);
			AttribMapping *attribs = batch->attribs;
			for (unsigned int i = 0; i < batch->attribcount; i++)
			{
				VertexLayoutElement *attrib = layout->getElement(i);
				attribs[i].shaderhandle = shader->getAttrib(attrib->name);
				attribs[i].components = attrib->components;
				attribs[i].stride = attrib->stride;
				attribs[i].type = attrib->type;
				attribs[i].address = layout->getSlotOffset(vertexcount,
				                                           attrib->vbslot)
				                   + attrib->offset;
			}
		}
		else
		{
			batch->attribcount = 0;
			batch->attribs = 0;
		}
		// Uniforms
		const UniformData::UniformMap &uniformdata = uniforms.getData();
		batch->uniformcount = uniformdata.size();
		unsigned int memsize = sizeof(UniformMapping) * batch->uniformcount;
		batch->uniforms = (UniformMapping*)memory->allocate(memsize);
		{
			unsigned int i = 0;
			for (UniformData::UniformMap::const_iterator it = uniformdata.begin();
			     it != uniformdata.end(); ++it, ++i)
			{
				batch->uniforms[i].type = it->second.getType();
				unsigned int size = ShaderVariableType::getSize(batch->uniforms[i].type);
				batch->uniforms[i].shaderhandle = shader->getUniform(it->second.getName());
				batch->uniforms[i].data = (float*)memory->allocate(size * sizeof(float));
				memcpy(batch->uniforms[i].data,
				       it->second.getData(),
				       size * sizeof(float));
			}
		}
		// Default uniforms
		batch->defuniformlocations = &shader->getDefaultUniformLocations();
		batch->defuniforms = defuniforms;
		// Compute derived uniforms
		// TODO: We can save memory copies here
		// TODO: We only need to compute those uniforms which are used in the shader
		// TODO: Those coming from the sequence only need to be computed once?
		math::Matrix4 transmat;
		for (unsigned int i = 0; i < 16; i++)
			transmat.m[i] = defuniforms->uniforms[DefaultUniformName::TransMatrix][i];
		math::Matrix4 transmatinv = transmat.inverse();
		for (unsigned int i = 0; i < 16; i++)
			defuniforms->uniforms[DefaultUniformName::TransMatrixInv][i] = transmatinv.m[i];
		math::Matrix4 projmat;
		for (unsigned int i = 0; i < 16; i++)
			projmat.m[i] = defuniforms->uniforms[DefaultUniformName::ProjMatrix][i];
		math::Matrix4 projmatinv = projmat.inverse();
		for (unsigned int i = 0; i < 16; i++)
			defuniforms->uniforms[DefaultUniformName::ProjMatrixInv][i] = projmatinv.m[i];
		math::Matrix4 worldmat = projmat * transmat;
		for (unsigned int i = 0; i < 16; i++)
			defuniforms->uniforms[DefaultUniformName::WorldMatrix][i] = worldmat.m[i];
		math::Matrix4 worldmatinv = transmatinv * projmatinv;
		for (unsigned int i = 0; i < 16; i++)
			defuniforms->uniforms[DefaultUniformName::WorldMatrixInv][i] = worldmatinv.m[i];
		// TODO: World normal matrix
		// Textures
		const std::vector<Material::TextureInfo> &textureinfo = material->getTextures();
		batch->texcount = textureinfo.size() + sequence->textures.size();
		if (batch->texcount > 0)
		{
			// TODO: Unify this?
			unsigned int memsize = sizeof(TextureEntry) * batch->texcount;
			TextureEntry *textures = (TextureEntry*)memory->allocate(memsize);
			// Normal texturesd
			for (unsigned int i = 0; i < textureinfo.size(); i++)
			{
				textures[i].shaderhandle = shader->getTexture(textureinfo[i].name);
				textures[i].textureindex = i;
				textures[i].texhandle = textureinfo[i].texture->getHandle();
				textures[i].type = textureinfo[i].texture->getTextureType();
			}
			// Sequence textures
			std::map<std::string, Texture::Ptr>::iterator it;
			unsigned int i = textureinfo.size();
			for (it = sequence->textures.begin(); it != sequence->textures.end(); it++)
			{
				textures[i].shaderhandle = shader->getTexture(it->first);
				textures[i].textureindex = i;
				textures[i].texhandle = it->second->getHandle();
				textures[i].type = it->second->getTextureType();
				i++;
			}
			batch->textures = textures;
		}
		else
		{
			batch->textures = 0;
		}
		return batch;
	}
}
}
