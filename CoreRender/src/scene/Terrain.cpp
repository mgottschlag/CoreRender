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

#include "CoreRender/scene/Terrain.hpp"
#include "CoreRender/res/ResourceManager.hpp"
#include "CoreRender/core/MemoryPool.hpp"
#include "CoreRender/render/Texture.hpp"
#include "CoreRender/GraphicsEngine.hpp"

#include <cstring>

namespace cr
{
namespace scene
{
	Terrain::Terrain(GraphicsEngine *graphics, const std::string& name)
		: Resource(graphics->getResourceManager(), name), graphics(graphics),
		displacement(0), normals(0), sizex(0), sizez(0), patchsize(0)
	{
		vertices = getManager()->createResource<render::VertexBuffer>("VertexBuffer");
		indices = getManager()->createResource<render::IndexBuffer>("IndexBuffer");
		layout = new render::VertexLayout(getManager()->getUploadManager(), 1);
		layout->setElement(0, getManager()->getNameRegistry().getAttrib("pos"),
			0, 3, 0, render::VertexElementType::Float, 0);
		displacementmap = getManager()->createResource<render::Texture>("Texture");
		displacementmap->setMipmapsEnabled(false);
		displacementmap->setFiltering(render::TextureFiltering::Nearest);
		/*normalmap = rmgr->createResource<render::Texture>("Texture");
		normalmap->setMipmapsEnabled(false);*/
	}
	Terrain::~Terrain()
	{
		if (displacement)
			delete[] displacement;
		if (normals)
			delete[] normals;
	}

	static bool isPowerOfTwo(unsigned int n)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html#DetermineIfPowerOf2
		return (n & (n - 1)) == 0;
	}
	static unsigned int log2(unsigned int n)
	{
		// http://graphics.stanford.edu/~seander/bithacks.html#IntegerLogDeBruijn
		static const int MultiplyDeBruijnBitPosition2[32] =
		{
		  0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8,
		  31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
		};
		return MultiplyDeBruijnBitPosition2[(uint32_t)(n * 0x077CB531U) >> 27];
	}

	void Terrain::render(render::RenderQueue &queue,
	                     math::Mat4f transmat,
	                     math::Vec3f camerapos)
	{
		// Allocate global uniforms
		core::MemoryPool *memory = queue.memory;
		render::CustomUniform texcoordscale;
		char *uniformname = (char*)memory->allocate(strlen("texCoordOffsetScale") + 1);
		strcpy(uniformname, "texCoordOffsetScale");
		float *uniformdata = (float*)memory->allocate(sizeof(float) * 4);
		// The texture coordinate 0 is not at the center, but at the edge of the
		// first texel, same applies to the last texel in a row/column, so we
		// have to scale/add to hit the texel centers
		uniformdata[0] = 0.5f / sizex;
		uniformdata[1] = 0.5f / sizez;
		uniformdata[2] = (float)(sizex - 1) / sizex;
		uniformdata[3] = (float)(sizez - 1) / sizez;
		texcoordscale.name = uniformname;
		texcoordscale.size = 4;
		texcoordscale.data = uniformdata;
		// For calculating the LODs we need the camera position in the local
		// coordinate system
		math::Mat4f transmatinv = transmat.inverse();
		math::Vec3f localcamerapos = transmatinv.transformPoint(camerapos);

		unsigned int maxdepth = log2((sizex - 1) / (patchsize - 1));

		float offsetscale[4] = {0, 0, 1, 1};
		renderRecursively(queue,
		                  transmat,
		                  maxdepth,
		                  localcamerapos,
		                  offsetscale,
		                  texcoordscale);
	}

	void Terrain::set(unsigned int sizex,
	                  unsigned int sizez,
	                  unsigned int patchsize,
	                  float *displacement)
	{
		if (sizex != sizez)
		{
			getManager()->getLog()->error("%s: Non-square terrains not implemented yet.",
			                              getName().c_str());
			return;
		}
		if (this->displacement)
			delete[] this->displacement;
		if (normals)
			delete[] normals;
		// The size has to be (2^n)+1 (the resulting terrain is 2^n units long)
		if (sizex < 3 || sizez == 3
		 || !isPowerOfTwo(sizex - 1)
		 || !isPowerOfTwo(sizez - 1))
		{
			getManager()->getLog()->error("%s: Invalid terrain size: %dx%d",
			                              getName().c_str(), sizex, sizez);
			return;
		}
		// Compute a fitting patch size
		if (sizex < patchsize || sizez < patchsize)
			patchsize = std::min<unsigned int>(sizex, sizez);
		else if (patchsize < 3 || !isPowerOfTwo(sizex - 1))
		{
			getManager()->getLog()->error("%s: Invalid patch size: %d",
			                              getName().c_str(), patchsize);
			return;
		}
		this->sizex = sizex;
		this->sizez = sizez;
		// Allocate displacement data
		// TODO: We should use an R or RGB texture format here
		this->displacement = new float[sizex * sizez * 4];
		normals = new float[sizex * sizez * 3];
		if (displacement)
		{
			for (unsigned int i = 0; i < sizex * sizez; i++)
			{
				this->displacement[i * 4] = 0.0f;
				this->displacement[i * 4 + 1] = displacement[i];
				this->displacement[i * 4 + 2] = 0.0f;
				this->displacement[i * 4 + 3] = 0.0f;
			}
			// Compute normals
			updateNormals(0, 0, sizex, sizez);
		}
		else
		{
			memset(this->displacement, 0, sizex * sizez * 4);
			for (unsigned int i = 0; i < sizex * sizez; i++)
			{
				this->displacement[i * 4] = 0.0f;
				this->displacement[i * 4 + 1] = (float)(i % sizex) / sizex;
				this->displacement[i * 4 + 2] = 0.0f;
			}
			for (unsigned int i = 0; i < sizex * sizez; i++)
			{
				normals[i * 3] = 0.0f;
				normals[i * 3 + 1] = 1.0f;
				normals[i * 3 + 2] = 0.0f;
			}
		}
		// Allocate textures
		displacementmap->set2D(sizex, sizez, render::TextureFormat::RGBA32F,
			render::TextureFormat::RGBA32F, this->displacement, true);
		// TODO
		// Update index/vertex buffers
		// This only has to be done if the patch size changed
		updateGeometry(patchsize);
		// Compute LOD offsets
		// TODO
	}

	void Terrain::setDisplacement(unsigned int x,
	                              unsigned int z,
	                              math::Vec3f displacement)
	{
	}
	void Terrain::setDisplacement(unsigned int x,
	                              unsigned int z,
	                              float height)
	{
		// TODO
	}
	math::Vec3f Terrain::getDisplacement(int x, int z)
	{
		// TODO
		return math::Vec3f(0, 0, 0);
	}

	bool Terrain::load()
	{
		finishLoading(false);
		return false;
	}

	bool Terrain::waitForLoading(bool recursive,
	                             bool highpriority)
	{
		if (!Resource::waitForLoading(recursive, highpriority))
			return false;
		// TODO: Wait for material
		return false;
	}

	void Terrain::updateNormals(unsigned int x,
	                            unsigned int z,
	                            unsigned int sizeX,
	                            unsigned int sizeZ)
	{
		// TODO
	}
	void Terrain::updateGeometry(unsigned int patchsize)
	{
		if (patchsize == this->patchsize)
			return;
		this->patchsize = patchsize;
		unsigned int lodcount = 1;
		// The vertices/indices are stored in one vertex/index buffer, so each
		// patch has an offset
		unsigned int vertexoffset = 0;
		unsigned int indexoffset = 0;
		while (lodcount < 4 && patchsize >= 3)
		{
			Patch patch;
			patch.mesh = graphics->createMesh();
			patch.mesh->setVertexBuffer(vertices);
			patch.mesh->setIndexBuffer(indices);
			patch.mesh->setVertexOffset(vertexoffset);
			patch.mesh->setVertexLayout(layout);
			patch.patchsize = patchsize;
			patch.indexoffset = indexoffset;
			// We need patchSize*patchSize vertices for the terrain surface and
			// patchSize*4-4 for the skirts to hide holes
			unsigned int vertexcount = patchsize * patchsize + 4 * patchsize - 4;
			unsigned int indextype;
			if (vertexcount >= 256)
				indextype = 2;
			else
				indextype = 1;
			patch.mesh->setIndexType(indextype);
			patch.mesh->setStartIndex(indexoffset / indextype);
			vertexoffset += vertexcount * sizeof(float) * 3;
			unsigned int indexcount = (patchsize - 1) * (patchsize - 1) * 6;
			indexcount += (patchsize - 1) * 4 * 6;
			// TODO: Triangle strips
			patch.mesh->setIndexCount(indexcount);
			indexoffset += indexcount * indextype;
			patches.push_back(patch);
			lodcount++;
			patchsize = ((patchsize - 1) / 2) + 1;
		}
		unsigned char *vertexdata = (unsigned char*)malloc(vertexoffset);
		unsigned char *indexdata = (unsigned char*)malloc(indexoffset);
		for (unsigned int j = 0; j < patches.size(); j++)
		{
			Patch &patch = patches[j];
			patchsize = patch.patchsize;
			float *vertices = (float*)& vertexdata[patch.mesh->getVertexOffset()];
			// Compute the surface vertices
			for (unsigned int i = 0; i < patchsize * patchsize; i++)
			{
				float x = (float)(i % patchsize) / (patchsize - 1);
				float z = (float)(i / patchsize) / (patchsize - 1);
				vertices[i * 3] = x;
				vertices[i * 3 + 1] = 0;
				vertices[i * 3 + 2] = z;
			}
			// Compute the skirt vertices
			unsigned int offset = patchsize * patchsize * 3;
			for (unsigned int i = 0; i < patchsize; i++)
			{
				float x = (float)i / (patchsize - 1);
				vertices[offset + i * 3] = x;
				vertices[offset + i * 3 + 1] = -1;
				vertices[offset + i * 3 + 2] = 0;
				vertices[offset + patchsize * 3 + i * 3] = x;
				vertices[offset + patchsize * 3 + i * 3 + 1] = -1;
				vertices[offset + patchsize * 3 + i * 3 + 2] = 1;
			}
			offset += patchsize * 2 * 3;
			for (unsigned int i = 0; i < patchsize - 2; i++)
			{
				float z = (float)(i+1) / (patchsize - 1);
				vertices[offset + i * 3] = 0;
				vertices[offset + i * 3 + 1] = -1;
				vertices[offset + i * 3 + 2] = z;
				vertices[offset + (patchsize - 2) * 3 + i * 3] = 1;
				vertices[offset + (patchsize - 2) * 3 + i * 3 + 1] = -1;
				vertices[offset + (patchsize - 2) * 3 + i * 3 + 2] = z;
			}
			if (patch.mesh->getIndexType() == 1)
			{
				// Compute the surface indices
				unsigned char *indices = (unsigned char*)& indexdata[patch.indexoffset];
				for (unsigned int i = 0; i < (patchsize - 1) * (patchsize - 1); i++)
				{
					unsigned int x = i % (patchsize - 1);
					unsigned int z = i / (patchsize - 1);
					unsigned int basevertex = z * patchsize + x;
					indices[i * 6] = basevertex;
					indices[i * 6 + 1] = basevertex + patchsize;
					indices[i * 6 + 2] = basevertex + 1;
					indices[i * 6 + 3] = basevertex + 1;
					indices[i * 6 + 4] = basevertex + patchsize;
					indices[i * 6 + 5] = basevertex + patchsize + 1;
				}
				// Compute the skirt indices
				unsigned int skirtoffset = patchsize * patchsize;
				unsigned int offset = (patchsize - 1) * (patchsize - 1) * 6;
				for (unsigned int i = 0; i < patchsize - 1; i++)
				{
					indices[offset + i * 6] = i;
					indices[offset + i * 6 + 1] = i + 1;
					indices[offset + i * 6 + 2] = skirtoffset + i;
					indices[offset + i * 6 + 3] = skirtoffset + i;
					indices[offset + i * 6 + 4] = i + 1;
					indices[offset + i * 6 + 5] = skirtoffset + i + 1;
				}
				offset += (patchsize - 1) * 6;
				skirtoffset += patchsize;
				for (unsigned int i = 0; i < patchsize - 1; i++)
				{
					indices[offset + i * 6] = (patchsize - 1) * patchsize + i;
					indices[offset + i * 6 + 1] = skirtoffset + i;
					indices[offset + i * 6 + 2] = (patchsize - 1) * patchsize + i + 1;
					indices[offset + i * 6 + 3] = (patchsize - 1) * patchsize + i + 1;
					indices[offset + i * 6 + 4] = skirtoffset + i;
					indices[offset + i * 6 + 5] = skirtoffset + i + 1;
				}
				offset += (patchsize - 1) * 6;
				skirtoffset += patchsize;
				indices[offset] = 0;
				indices[offset + 2] = patchsize;
				indices[offset + 1] = patchsize * patchsize;
				indices[offset + 4] = patchsize * patchsize;
				indices[offset + 3] = patchsize;
				indices[offset + 5] = skirtoffset;
				offset += 6;
				for (unsigned int i = 0; i < patchsize - 3; i++)
				{
					indices[offset + i * 6] = (i + 1) * patchsize;
					indices[offset + i * 6 + 1] = skirtoffset + i;
					indices[offset + i * 6 + 2] = (i + 2) * patchsize;
					indices[offset + i * 6 + 3] = (i + 2) * patchsize;
					indices[offset + i * 6 + 4] = skirtoffset + i;
					indices[offset + i * 6 + 5] = skirtoffset + i + 1;
				}
				offset += (patchsize - 3) * 6;
				indices[offset] = (patchsize - 2) * patchsize;
				indices[offset + 2] = (patchsize - 1) * patchsize;
				indices[offset + 1] = skirtoffset + patchsize - 3;
				indices[offset + 4] = skirtoffset + patchsize - 3;
				indices[offset + 3] = (patchsize - 1) * patchsize;
				indices[offset + 5] = patchsize * (patchsize + 1);
				offset += 6;
				skirtoffset += patchsize - 2;
				indices[offset] = patchsize - 1;
				indices[offset + 1] = patchsize * 2 - 1;
				indices[offset + 2] = patchsize * (patchsize + 1) - 1;
				indices[offset + 3] = patchsize * (patchsize + 1) - 1;
				indices[offset + 4] = patchsize * 2 - 1;
				indices[offset + 5] = skirtoffset;
				offset += 6;
				for (unsigned int i = 0; i < patchsize - 3; i++)
				{
					indices[offset + i * 6] = (i + 1) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 1] = (i + 2) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 2] = skirtoffset + i;
					indices[offset + i * 6 + 3] = skirtoffset + i;
					indices[offset + i * 6 + 4] = (i + 2) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 5] = skirtoffset + i + 1;
				}
				offset += (patchsize - 3) * 6;
				indices[offset] = (patchsize - 1) * patchsize - 1;
				indices[offset + 1] = patchsize * patchsize - 1;
				indices[offset + 2] = skirtoffset + patchsize - 3;
				indices[offset + 3] = skirtoffset + patchsize - 3;
				indices[offset + 4] = patchsize * patchsize - 1;
				indices[offset + 5] = patchsize * (patchsize + 1) + patchsize - 1;
			}
			else
			{
				// Compute the surface indices
				unsigned short *indices = (unsigned short*)& indexdata[patch.indexoffset];
				for (unsigned int i = 0; i < (patchsize - 1) * (patchsize - 1); i++)
				{
					unsigned int x = i % (patchsize - 1);
					unsigned int z = i / (patchsize - 1);
					unsigned int basevertex = z * patchsize + x;
					indices[i * 6] = basevertex;
					indices[i * 6 + 1] = basevertex + patchsize;
					indices[i * 6 + 2] = basevertex + 1;
					indices[i * 6 + 3] = basevertex + 1;
					indices[i * 6 + 4] = basevertex + patchsize;
					indices[i * 6 + 5] = basevertex + patchsize + 1;
				}
				// Compute the skirt indices
				unsigned int skirtOffset = patchsize * patchsize;
				unsigned int offset = (patchsize - 1) * (patchsize - 1) * 6;
				for (unsigned int i = 0; i < patchsize - 1; i++)
				{
					indices[offset + i * 6] = i;
					indices[offset + i * 6 + 1] = i + 1;
					indices[offset + i * 6 + 2] = skirtOffset + i;
					indices[offset + i * 6 + 3] = skirtOffset + i;
					indices[offset + i * 6 + 4] = i + 1;
					indices[offset + i * 6 + 5] = skirtOffset + i + 1;
				}
				offset += (patchsize - 1) * 6;
				skirtOffset += patchsize;
				for (unsigned int i = 0; i < patchsize - 1; i++)
				{
					indices[offset + i * 6] = (patchsize - 1) * patchsize + i;
					indices[offset + i * 6 + 1] = skirtOffset + i;
					indices[offset + i * 6 + 2] = (patchsize - 1) * patchsize + i + 1;
					indices[offset + i * 6 + 3] = (patchsize - 1) * patchsize + i + 1;
					indices[offset + i * 6 + 4] = skirtOffset + i;
					indices[offset + i * 6 + 5] = skirtOffset + i + 1;
				}
				offset += (patchsize - 1) * 6;
				skirtOffset += patchsize;
				indices[offset] = 0;
				indices[offset + 2] = patchsize;
				indices[offset + 1] = patchsize * patchsize;
				indices[offset + 4] = patchsize * patchsize;
				indices[offset + 3] = patchsize;
				indices[offset + 5] = skirtOffset;
				offset += 6;
				for (unsigned int i = 0; i < patchsize - 3; i++)
				{
					indices[offset + i * 6] = (i + 1) * patchsize;
					indices[offset + i * 6 + 1] = skirtOffset + i;
					indices[offset + i * 6 + 2] = (i + 2) * patchsize;
					indices[offset + i * 6 + 3] = (i + 2) * patchsize;
					indices[offset + i * 6 + 4] = skirtOffset + i;
					indices[offset + i * 6 + 5] = skirtOffset + i + 1;
				}
				offset += (patchsize - 3) * 6;
				indices[offset] = (patchsize - 2) * patchsize;
				indices[offset + 2] = (patchsize - 1) * patchsize;
				indices[offset + 1] = skirtOffset + patchsize - 3;
				indices[offset + 4] = skirtOffset + patchsize - 3;
				indices[offset + 3] = (patchsize - 1) * patchsize;
				indices[offset + 5] = patchsize * (patchsize + 1);
				offset += 6;
				skirtOffset += patchsize - 2;
				indices[offset] = patchsize - 1;
				indices[offset + 1] = patchsize * 2 - 1;
				indices[offset + 2] = patchsize * (patchsize + 1) - 1;
				indices[offset + 3] = patchsize * (patchsize + 1) - 1;
				indices[offset + 4] = patchsize * 2 - 1;
				indices[offset + 5] = skirtOffset;
				offset += 6;
				for (unsigned int i = 0; i < patchsize - 3; i++)
				{
					indices[offset + i * 6] = (i + 1) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 1] = (i + 2) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 2] = skirtOffset + i;
					indices[offset + i * 6 + 3] = skirtOffset + i;
					indices[offset + i * 6 + 4] = (i + 2) * patchsize + patchsize - 1;
					indices[offset + i * 6 + 5] = skirtOffset + i + 1;
				}
				offset += (patchsize - 3) * 6;
				indices[offset] = (patchsize - 1) * patchsize - 1;
				indices[offset + 1] = patchsize * patchsize - 1;
				indices[offset + 2] = skirtOffset + patchsize - 3;
				indices[offset + 3] = skirtOffset + patchsize - 3;
				indices[offset + 4] = patchsize * patchsize - 1;
				indices[offset + 5] = patchsize * (patchsize + 1) + patchsize - 1;
			}
		}
		// Create the geometry buffers
		vertices->set(vertexoffset,
		              vertexdata,
		              render::VertexBufferUsage::Static,
		              false);
		indices->set(indexoffset,
		             indexdata,
		             false);
	}

	void Terrain::renderPatch(render::RenderQueue &queue,
	                          math::Mat4f transmat,
	                          unsigned int lod,
	                          float *offsetscale,
	                          render::CustomUniform &texcoordscale)
	{
		render::Batch *batch = queue.prepareBatch(material.get(), patches[lod].mesh.get(), false, false);
		// TODO: This overwrites existing material uniforms
		core::MemoryPool *memory = queue.memory;
		batch->customuniformcount = 2;
		void *ptr = memory->allocate(sizeof(render::CustomUniform) * 2);
		render::CustomUniform *uniform = (render::CustomUniform*)ptr;
		char *uniformname = (char*)memory->allocate(strlen("terrainOffsetScale") + 1);
		strcpy(uniformname, "terrainOffsetScale");
		float *uniformdata = (float*)memory->allocate(sizeof(float) * 4);
		memcpy(uniformdata, offsetscale, sizeof(float) * 4);
		uniform[0].name = uniformname;
		uniform[0].size = 4;
		uniform[0].data = uniformdata;
		uniform[1] = texcoordscale;
		batch->customuniforms = uniform;
		batch->transmat = transmat;
		// Add batch to the render queuemesh
		tbb::mutex::scoped_lock lock(queue.batchmutex);
		queue.batches.push_back(batch);
	}

	void Terrain::renderRecursively(render::RenderQueue &queue,
	                                math::Mat4f transmat,
	                                unsigned int maxdepth,
	                                math::Vec3f camerapos,
	                                float *offsetscale,
	                                render::CustomUniform &texcoordscale)
	{
		// TODO: Clipping
		math::Vec2f center(offsetscale[0] + offsetscale[2] * 0.5f,
		                   offsetscale[1] + offsetscale[3] * 0.5f);
		float cameradist = (center - math::Vec2f(camerapos.x, camerapos.z)).getLength();
		if (cameradist < offsetscale[2] * 1.5 && maxdepth > 0)
		{
			float offsetscale1[4] =
			{
				offsetscale[0],
				offsetscale[1],
				offsetscale[2] * 0.5f,
				offsetscale[3] * 0.5f
			};
			float offsetscale2[4] =
			{
				offsetscale[0] + offsetscale[2] * 0.5f,
				offsetscale[1],
				offsetscale[2] * 0.5f,
				offsetscale[3] * 0.5f
			};
			float offsetscale3[4] =
			{
				offsetscale[0],
				offsetscale[1] + offsetscale[3] * 0.5f,
				offsetscale[2] * 0.5f,
				offsetscale[3] * 0.5f
			};
			float offsetscale4[4] =
			{
				offsetscale[0] + offsetscale[2] * 0.5f,
				offsetscale[1] + offsetscale[3] * 0.5f,
				offsetscale[2] * 0.5f,
				offsetscale[3] * 0.5f
			};
			renderRecursively(queue, transmat, maxdepth - 1, camerapos,
				offsetscale1, texcoordscale);
			renderRecursively(queue, transmat, maxdepth - 1, camerapos,
				offsetscale2, texcoordscale);
			renderRecursively(queue, transmat, maxdepth - 1, camerapos,
				offsetscale3, texcoordscale);
			renderRecursively(queue, transmat, maxdepth - 1, camerapos,
				offsetscale4, texcoordscale);
		}
		else
		{
			int lodcount = patches.size();
			// TODO: Faster conversion
			int lodindex = math::Math::ftoi(cameradist / offsetscale[2]) - 1;
			if (lodindex < 0)
				lodindex = 0;
			if (lodindex >= lodcount)
				lodindex = lodcount - 1;
			renderPatch(queue, transmat, lodindex, offsetscale, texcoordscale);
		}
	}
}
}

