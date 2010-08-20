
#include "tinyxml.h"
#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>
#include <iostream>
#include <vector>
#include <fstream>

struct VertexAttribFlags
{
	enum List
	{
		HasPositions = 0x1,
		HasNormals = 0x2,
		HasTangents = 0x4,
		HasBitangents = 0x8,
		HasTexCoords = 0x10,
		HasJoints = 0x20,
		HasColors = 0x40
	};
};
struct VertexAttribInfo
{
	unsigned int flags;
	unsigned char stride;
	unsigned char posoffset;
	unsigned char normaloffset;
	unsigned char tangentoffset;
	unsigned char bitangentoffset;
	unsigned char jointoffset;
	unsigned char jointweightoffset;
	unsigned char texcoordcount;
	unsigned char texcoordoffset[8];
	unsigned char texcoordsize[8];
	unsigned char coloroffset[4];
	unsigned char colorcount;
};
struct GeometryDataInfo
{
	unsigned int vertexoffset;
	unsigned int vertexsize;
	unsigned int indexoffset;
	unsigned int indexsize;
	unsigned int indextype;
	unsigned int indexcount;
	unsigned int basevertex;
};

struct Mesh
{
	VertexAttribInfo attribs;
	GeometryDataInfo geom;
};

struct OutputInfo
{
	OutputInfo()
		: vertexdatasize(0), vertexdata(0), indexdatasize(0), indexdata(0)
	{
	}
	~OutputInfo()
	{
		if (vertexdata)
			free(vertexdata);
		if (indexdata)
			free(indexdata);
	}

	unsigned int allocateVertexData(unsigned int size, unsigned int alignment = 8)
	{
		unsigned int alignoffset = 0;
		if (vertexdatasize % alignment)
		{
			alignoffset = alignment - (vertexdatasize % alignment);
		}
		unsigned int allocoffset = vertexdatasize + alignoffset;
		unsigned int newsize = vertexdatasize + size + alignoffset;
		vertexdata = realloc(vertexdata, newsize);
		void *newdata = (char*)vertexdata + vertexdatasize;
		vertexdatasize = newsize;
		memset(newdata, 0, size);
		return allocoffset;
	}
	unsigned int allocateIndexData(unsigned int size, unsigned int alignment = 4)
	{
		unsigned int alignoffset = 0;
		if (indexdatasize % alignment)
		{
			alignoffset = alignment - (indexdatasize % alignment);
		}
		unsigned int allocoffset = indexdatasize + alignoffset;
		unsigned int newsize = indexdatasize + size + alignoffset;
		indexdata = realloc(indexdata, newsize);
		void *newdata = (char*)indexdata + indexdatasize;
		indexdatasize = newsize;
		memset(newdata, 0, size);
		return allocoffset;
	}

	std::vector<Mesh> meshes;
	unsigned int vertexdatasize;
	void *vertexdata;
	unsigned int indexdatasize;
	void *indexdata;
};

int main(int argc, char **argv)
{
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <modelfile>" << std::endl;
		return -1;
	}
	Assimp::Importer importer;
	// Open file
	const aiScene *scene = importer.ReadFile(argv[1],
	                                         aiProcess_CalcTangentSpace |
	                                         aiProcess_Triangulate |
	                                         aiProcess_JoinIdenticalVertices |
	                                         aiProcess_SortByPType);
	if (!scene)
	{
		std::cerr << "Importing the model failed: " << importer.GetErrorString() << std::endl;
		return -1;
	}
	if (!scene->HasMeshes())
	{
		std::cerr << "Model contains no meshes." << std::endl;
		return -1;
	}
	OutputInfo output;
	// Collect meshes
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *meshsrc = scene->mMeshes[i];
		if (meshsrc->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		{
			std::cout << "Warning: Non-triangle mesh." << std::endl;
			continue;
		}
		Mesh meshinfo;
		VertexAttribInfo &attribs = meshinfo.attribs;
		memset(&meshinfo, 0, sizeof(meshinfo));
		// Get attribs
		if (meshsrc->HasPositions())
			attribs.flags |= VertexAttribFlags::HasPositions;
		if (meshsrc->HasNormals())
			attribs.flags |= VertexAttribFlags::HasNormals;
		if (meshsrc->HasTangentsAndBitangents())
			attribs.flags |= VertexAttribFlags::HasTangents |
			                          VertexAttribFlags::HasBitangents;
		if (meshsrc->HasBones())
			attribs.flags |= VertexAttribFlags::HasJoints;
		unsigned int uvcount = 0;
		unsigned int uvsize[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		unsigned int uvindex[AI_MAX_NUMBER_OF_TEXTURECOORDS];
		for (unsigned int j = 0; j < AI_MAX_NUMBER_OF_TEXTURECOORDS; j++)
		{
			if (meshsrc->HasTextureCoords(j))
			{
				uvsize[uvcount] = meshsrc->mNumUVComponents[j];
				uvindex[uvcount] = j;
				uvcount++;
				if (uvcount == 8)
					break;
			}
		}
		if (uvcount > 0)
			attribs.flags |= VertexAttribFlags::HasTexCoords;
		attribs.texcoordcount = uvcount;
		unsigned int colorcount = 0;
		unsigned int colorindex[AI_MAX_NUMBER_OF_COLOR_SETS];
		for (unsigned int j = 0; j < AI_MAX_NUMBER_OF_COLOR_SETS; j++)
		{
			if (meshsrc->HasVertexColors(j))
			{
				colorindex[colorcount] = j;
				colorcount++;
				if (colorcount == 4)
					break;
			}
		}
		if (colorcount > 0)
			attribs.flags |= VertexAttribFlags::HasColors;
		attribs.colorcount = colorcount;
		// Compute stride and offsets
		unsigned int offset = 0;
		if (meshsrc->HasPositions())
		{
			attribs.posoffset = offset;
			offset += 3 * sizeof(float);
		}
		if (meshsrc->HasNormals())
		{
			attribs.normaloffset = offset;
			offset += 3 * sizeof(float);
		}
		if (meshsrc->HasTangentsAndBitangents())
		{
			attribs.tangentoffset = offset;
			offset += 3 * sizeof(float);
			attribs.bitangentoffset = offset;
			offset += 3 * sizeof(float);
		}
		for (unsigned int j = 0; j < uvcount; j++)
		{
			attribs.texcoordoffset[j] = offset;
			attribs.texcoordsize[j] = uvsize[j];
			offset += uvsize[j] * sizeof(float);
		}
		for (unsigned int j = 0; j < colorcount; j++)
		{
			attribs.coloroffset[j] = offset;
			offset += 4 * sizeof(float);
		}
		// TODO: Joints
		attribs.stride = offset;
		// Round up stride
		// TODO
		// Allocate data
		unsigned int size = meshsrc->mNumVertices * attribs.stride;
		meshinfo.geom.vertexoffset = output.allocateVertexData(size);
		meshinfo.geom.vertexsize = size;
		// Fill data in
		float *vertices = (float*)((char*)output.vertexdata + meshinfo.geom.vertexoffset);
		if (meshsrc->HasPositions())
		{
			float *pos = (float*)((char*)vertices + attribs.posoffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				pos[0] = meshsrc->mVertices[j].x;
				pos[1] = meshsrc->mVertices[j].y;
				pos[2] = meshsrc->mVertices[j].z;
				pos = (float*)((char*)pos + meshinfo.attribs.stride);
			}
		}
		if (meshsrc->HasNormals())
		{
			float *normal = (float*)((char*)vertices + attribs.normaloffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				normal[0] = meshsrc->mNormals[j].x;
				normal[1] = meshsrc->mNormals[j].y;
				normal[2] = meshsrc->mNormals[j].z;
				normal = (float*)((char*)normal + meshinfo.attribs.stride);
			}
		}
		if (meshsrc->HasTangentsAndBitangents())
		{
			float *tangent = (float*)((char*)vertices + attribs.tangentoffset);
			float *bitangent = (float*)((char*)vertices + attribs.bitangentoffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				tangent[0] = meshsrc->mTangents[j].x;
				tangent[1] = meshsrc->mTangents[j].y;
				tangent[2] = meshsrc->mTangents[j].z;
				bitangent[0] = meshsrc->mBitangents[j].x;
				bitangent[1] = meshsrc->mBitangents[j].y;
				bitangent[2] = meshsrc->mBitangents[j].z;
				tangent = (float*)((char*)tangent + meshinfo.attribs.stride);
				bitangent = (float*)((char*)bitangent + meshinfo.attribs.stride);
			}
		}
		for (unsigned int i = 0; i < uvcount; i++)
		{
			float *texcoord = (float*)((char*)vertices + attribs.texcoordoffset[i]);
			aiVector3D *texcoordsrc = meshsrc->mTextureCoords[uvindex[i]];
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				if (attribs.texcoordsize[i] > 0)
					texcoord[0] = texcoordsrc[j].x;
				if (attribs.texcoordsize[i] > 1)
					texcoord[1] = texcoordsrc[j].y;
				if (attribs.texcoordsize[i] > 2)
					texcoord[2] = texcoordsrc[j].z;
				texcoord = (float*)((char*)texcoord + meshinfo.attribs.stride);
			}
		}
		for (unsigned int i = 0; i < colorcount; i++)
		{
			float *color = (float*)((char*)vertices + attribs.coloroffset[i]);
			aiColor4D *colorsrc = meshsrc->mColors[colorindex[i]];
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				color[0] = colorsrc[j].r;
				color[1] = colorsrc[j].g;
				color[2] = colorsrc[j].b;
				color[3] = colorsrc[j].a;
				color = (float*)((char*)color + meshinfo.attribs.stride);
			}
		}
		// TODO: Joints
		// Get index count
		unsigned int indexcount = 0;
		unsigned int minindex = 0xFFFFFFFF;
		unsigned int maxindex = 0;
		for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
		{
			if (meshsrc->mFaces[j].mNumIndices != 3)
			{
				std::cout << "Error: meshsrc->mFaces[j].mNumIndices != 3" << std::endl;
				return -1;
			}
			// Calculate min/max indices - necessary to derive the index size
			for (unsigned int k = 0; k < 3; k++)
			{
				if (meshsrc->mFaces[j].mIndices[k] > maxindex)
					maxindex = meshsrc->mFaces[j].mIndices[k];
				if (meshsrc->mFaces[j].mIndices[k] < minindex)
					minindex = meshsrc->mFaces[j].mIndices[k];
			}
			indexcount += 3;
		}
		// Allocate index data
		meshinfo.geom.indexcount = indexcount;
		// TODO: Use smaller indices if possible
		if (maxindex - minindex < 256)
			meshinfo.geom.indextype = 1;
		else if (maxindex - minindex < 65536)
			meshinfo.geom.indextype = 2;
		else
			meshinfo.geom.indextype = 4;
		meshinfo.geom.indexsize = meshinfo.geom.indextype * meshinfo.geom.indexcount;
		meshinfo.geom.indexoffset = output.allocateIndexData(meshinfo.geom.indexsize,
		                                                     meshinfo.geom.indextype);
		meshinfo.geom.basevertex = minindex;
		// Fill in indices
		if (meshinfo.geom.indextype == 1)
		{
			unsigned char *indices = (unsigned char*)output.indexdata
			                       + meshinfo.geom.indexoffset;
			for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
			{
				indices[j * 3] = meshsrc->mFaces[j].mIndices[0] - minindex;
				indices[j * 3 + 1] = meshsrc->mFaces[j].mIndices[1] - minindex;
				indices[j * 3 + 2] = meshsrc->mFaces[j].mIndices[2] - minindex;
			}
		}
		else if (meshinfo.geom.indextype == 2)
		{
			unsigned short *indices = (unsigned short*)((char*)output.indexdata
			                        + meshinfo.geom.indexoffset);
			for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
			{
				indices[j * 3] = meshsrc->mFaces[j].mIndices[0] - minindex;
				indices[j * 3 + 1] = meshsrc->mFaces[j].mIndices[1] - minindex;
				indices[j * 3 + 2] = meshsrc->mFaces[j].mIndices[2] - minindex;
			}
		}
		else
		{
			unsigned int *indices = (unsigned int*)((char*)output.indexdata
			                      + meshinfo.geom.indexoffset);
			for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
			{
				indices[j * 3] = meshsrc->mFaces[j].mIndices[0] - minindex;
				indices[j * 3 + 1] = meshsrc->mFaces[j].mIndices[1] - minindex;
				indices[j * 3 + 2] = meshsrc->mFaces[j].mIndices[2] - minindex;
			}
		}
		// Add mesh to file
		output.meshes.push_back(meshinfo);
	}
	// Get base file name
	std::string filename = argv[1];
	if (filename.rfind(".") != std::string::npos)
		filename = filename.substr(0, filename.rfind("."));
	std::string relfilename = filename;
	if (relfilename.rfind("/") != std::string::npos)
		relfilename = relfilename.substr(relfilename.rfind("/") + 1);
	// Save geometry file
	{
		std::ofstream file((filename + ".geo").c_str());
		if (!file)
		{
			std::cerr << "Could not open " << filename + ".geo" << std::endl;
			return -1;
		}
		// File header
		char tag[4] = "CRG";
		file.write(tag, 4);
		unsigned int version = 0;
		file.write((char*)&version, 4);
		// TODO: Joints
		file.write((char*)&output.vertexdatasize, 4);
		file.write((char*)&output.indexdatasize, 4);
		unsigned int meshcount = output.meshes.size();
		file.write((char*)&meshcount, 4);
		// Write vertex/index data
		file.write((char*)output.vertexdata, output.vertexdatasize);
		file.write((char*)output.indexdata, output.indexdatasize);
		// Write mesh info
		for (unsigned int i = 0; i < meshcount; i++)
		{
			Mesh &mesh = output.meshes[i];
			file.write((char*)&mesh.attribs, sizeof(mesh.attribs));
			file.write((char*)&mesh.geom, sizeof(mesh.geom));
		}
	}
	// Create XML model file
	TiXmlDocument xml((filename + ".model.xml").c_str());
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0", "", "");
	xml.LinkEndChild(dec);
	TiXmlElement *root = new TiXmlElement("Model");
	xml.LinkEndChild(root);
	root->SetAttribute("geometry",(relfilename + ".geo").c_str());
	for (unsigned int i = 0; i < output.meshes.size(); i++)
	{
		TiXmlElement *mesh = new TiXmlElement("Mesh");
		root->LinkEndChild(mesh);
		mesh->SetAttribute("index", i);
		mesh->SetAttribute("material", "/materials/Model.material.xml");
	}
	// TODO: Joints
	// Save file
	xml.SaveFile();
	return 0;
}
