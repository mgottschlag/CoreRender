
#include "tinyxml.h"
#include "../../../CoreRender/include/CoreRender/render/GeometryFile.hpp"
#include "../../../CoreRender/include/CoreRender/render/AnimationFile.hpp"
#include <assimp.hpp>
#include <aiScene.h>
#include <aiPostProcess.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <queue>
#include <sstream>

using namespace cr;
using namespace render;

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

	std::vector<GeometryFile::Batch> batches;
	unsigned int vertexdatasize;
	void *vertexdata;
	unsigned int indexdatasize;
	void *indexdata;
};

void writeNode(const aiScene *scene,
               aiNode *node,
               TiXmlElement *xml,
               const std::string &basename)
{
	// Create XML element
	TiXmlElement *nodeelem = new TiXmlElement("Node");
	nodeelem->SetAttribute("name", node->mName.data);
	xml->LinkEndChild(nodeelem);
	// Write transformation
	aiMatrix4x4 transmat = node->mTransformation;
	if (!transmat.IsIdentity())
	{
		TiXmlElement *transformation = new TiXmlElement("Transformation");
		nodeelem->LinkEndChild(transformation);
		std::ostringstream matstream;
		for (unsigned int i = 0; i < 16; i++)
		{
			matstream << transmat[i%4][i / 4];
			if (i != 15)
				matstream << ", ";
		}
		transformation->LinkEndChild(new TiXmlText(matstream.str().c_str()));
	}
	// Write meshes
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		TiXmlElement *mesh = new TiXmlElement("Mesh");
		nodeelem->LinkEndChild(mesh);
		mesh->SetAttribute("index", node->mMeshes[i]);
		mesh->SetAttribute("material", (basename + ".material.xml").c_str());
	}
	// Recursively write children
	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		writeNode(scene, node->mChildren[i], nodeelem, basename);
	}
}

AnimationFile::Frame getAnimationFrame(aiNodeAnim *channel, unsigned int time)
{
	AnimationFile::Frame frame;
	// Default values
	frame.position[0] = frame.position[1] = frame.position[2] = 0.0f;
	frame.scale[0] = frame.scale[1] = frame.scale[2] = 1.0f;
	frame.rotation[0] = frame.rotation[1] = frame.rotation[2] = 0.0f;
	frame.rotation[3] = 1.0f;
	// Get position
	if (channel->mNumPositionKeys > 0)
	{
		int secondkey = -1;
		int firstkey = -1;
		for (unsigned int i = 0; i < channel->mNumPositionKeys; i++)
		{
			if (channel->mPositionKeys[i].mTime >= (double)time)
			{
				secondkey = i;
				break;
			}
			else
			{
				firstkey = i;
			}
		}
		if (secondkey == -1)
		{
			// Just take the last key
			frame.position[0] = channel->mPositionKeys[firstkey].mValue.x;
			frame.position[1] = channel->mPositionKeys[firstkey].mValue.y;
			frame.position[2] = channel->mPositionKeys[firstkey].mValue.z;
		}
		else if (firstkey == -1)
		{
			// Just take the first key
			frame.position[0] = channel->mPositionKeys[0].mValue.x;
			frame.position[1] = channel->mPositionKeys[0].mValue.y;
			frame.position[2] = channel->mPositionKeys[0].mValue.z;
		}
		else
		{
			// Interpolate
			aiVectorKey &first = channel->mPositionKeys[firstkey];
			aiVectorKey &second = channel->mPositionKeys[secondkey];
			float factor = ((double)time - first.mTime) / (second.mTime - first.mTime);
			frame.position[0] = factor * second.mValue.x + (1 - factor) * first.mValue.x;
			frame.position[1] = factor * second.mValue.y + (1 - factor) * first.mValue.y;
			frame.position[2] = factor * second.mValue.z + (1 - factor) * first.mValue.z;
		}
	}
	// Get rotation
	if (channel->mNumRotationKeys > 0)
	{
		int secondkey = -1;
		int firstkey = -1;
		for (unsigned int i = 0; i < channel->mNumRotationKeys; i++)
		{
			if (channel->mRotationKeys[i].mTime >= (double)time)
			{
				secondkey = i;
				break;
			}
			else
			{
				firstkey = i;
			}
		}
		if (secondkey == -1)
		{
			// Just take the last key
			aiQuaternion rot = channel->mRotationKeys[firstkey].mValue;
			frame.rotation[0] = rot.x;
			frame.rotation[1] = rot.y;
			frame.rotation[2] = rot.z;
			frame.rotation[3] = rot.w;
		}
		else if (firstkey == -1)
		{
			// Just take the first key
			aiQuaternion rot = channel->mRotationKeys[0].mValue;
			frame.rotation[0] = rot.x;
			frame.rotation[1] = rot.y;
			frame.rotation[2] = rot.z;
			frame.rotation[3] = rot.w;
		}
		else
		{
			// Interpolate
			aiQuatKey &first = channel->mRotationKeys[firstkey];
			aiQuatKey &second = channel->mRotationKeys[secondkey];
			float factor = ((double)time - first.mTime) / (second.mTime - first.mTime);
			aiQuaternion rot1 = channel->mRotationKeys[firstkey].mValue;
			aiQuaternion rot2 = channel->mRotationKeys[secondkey].mValue;
			aiQuaternion rot;
			aiQuaternion::Interpolate(rot, rot1, rot2, factor);
			frame.rotation[0] = rot.x;
			frame.rotation[1] = rot.y;
			frame.rotation[2] = rot.z;
			frame.rotation[3] = rot.w;
		}
	}
	// Get scale
	if (channel->mNumScalingKeys > 0)
	{
		int secondkey = -1;
		int firstkey = -1;
		for (unsigned int i = 0; i < channel->mNumScalingKeys; i++)
		{
			if (channel->mScalingKeys[i].mTime >= (double)time)
			{
				secondkey = i;
				break;
			}
			else
			{
				firstkey = i;
			}
		}
		if (secondkey == -1)
		{
			// Just take the last key
			frame.scale[0] = channel->mScalingKeys[firstkey].mValue.x;
			frame.scale[1] = channel->mScalingKeys[firstkey].mValue.y;
			frame.scale[2] = channel->mScalingKeys[firstkey].mValue.z;
		}
		else if (firstkey == -1)
		{
			// Just take the first key
			frame.scale[0] = channel->mScalingKeys[0].mValue.x;
			frame.scale[1] = channel->mScalingKeys[0].mValue.y;
			frame.scale[2] = channel->mScalingKeys[0].mValue.z;
		}
		else
		{
			// Interpolate
			aiVectorKey &first = channel->mScalingKeys[firstkey];
			aiVectorKey &second = channel->mScalingKeys[secondkey];
			float factor = ((double)time - first.mTime) / (second.mTime - first.mTime);
			frame.scale[0] = factor * second.mValue.x + (1 - factor) * first.mValue.x;
			frame.scale[1] = factor * second.mValue.y + (1 - factor) * first.mValue.y;
			frame.scale[2] = factor * second.mValue.z + (1 - factor) * first.mValue.z;
		}
	}
	return frame;
}

int main(int argc, char **argv)
{
	// TODO: Make this a switchable setting
	bool swapyz = false;
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
	                                         aiProcess_SortByPType |
	                                         aiProcess_FlipWindingOrder);
	if (!scene)
	{
		std::cerr << "Importing the model failed: " << importer.GetErrorString() << std::endl;
		return -1;
	}
	if (!scene->HasMeshes())
	{
		std::cerr << "Model contains no batches." << std::endl;
		return -1;
	}
	OutputInfo output;
	// Collect meshes
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *meshsrc = scene->mMeshes[i];
		if (meshsrc->mPrimitiveTypes != aiPrimitiveType_TRIANGLE)
		{
			std::cout << "Warning: Non-triangle batch." << std::endl;
			continue;
		}
		GeometryFile::Batch batch;
		GeometryFile::AttribInfo &attribs = batch.attribs;
		memset(&batch, 0, sizeof(batch));
		// Get attribs
		if (meshsrc->HasPositions())
			attribs.flags |= GeometryFile::AttribFlags::HasPositions;
		if (meshsrc->HasNormals())
			attribs.flags |= GeometryFile::AttribFlags::HasNormals;
		if (meshsrc->HasTangentsAndBitangents())
			attribs.flags |= GeometryFile::AttribFlags::HasTangents |
			                 GeometryFile::AttribFlags::HasBitangents;
		if (meshsrc->HasBones())
			attribs.flags |= GeometryFile::AttribFlags::HasJoints;
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
			attribs.flags |= GeometryFile::AttribFlags::HasTexCoords;
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
			attribs.flags |= GeometryFile::AttribFlags::HasColors;
		attribs.colorcount = colorcount;
		// Compute stride and offsets
		unsigned int offset = 0;
		if (meshsrc->HasPositions())
		{
			std::cout << "Position offset: " << offset << " bytes." << std::endl;
			attribs.posoffset = offset;
			offset += 3 * sizeof(float);
		}
		if (meshsrc->HasNormals())
		{
			std::cout << "Normal offset: " << offset << " bytes." << std::endl;
			attribs.normaloffset = offset;
			offset += 3 * sizeof(float);
		}
		if (meshsrc->HasTangentsAndBitangents())
		{
			std::cout << "Tangent offset: " << offset << " bytes." << std::endl;
			attribs.tangentoffset = offset;
			offset += 3 * sizeof(float);
			std::cout << "Bitangent offset: " << offset << " bytes." << std::endl;
			attribs.bitangentoffset = offset;
			offset += 3 * sizeof(float);
		}
		for (unsigned int j = 0; j < uvcount; j++)
		{
			std::cout << "Texcoord " << j << " offset: " << offset << " bytes." << std::endl;
			attribs.texcoordoffset[j] = offset;
			attribs.texcoordsize[j] = uvsize[j];
			offset += uvsize[j] * sizeof(float);
		}
		for (unsigned int j = 0; j < colorcount; j++)
		{
			std::cout << "Color " << j << " offset: " << offset << " bytes." << std::endl;
			attribs.coloroffset[j] = offset;
			offset += 4 * sizeof(float);
		}
		// Joints
		if (meshsrc->HasBones())
		{
			std::cout << "Joint index offset: " << offset << " bytes." << std::endl;
			attribs.jointoffset = offset;
			offset += 4;
			std::cout << "Joint weight offset: " << offset << " bytes." << std::endl;
			attribs.jointweightoffset = offset;
			offset += 4 * sizeof(float);
		}
		// Compute stride
		attribs.stride = offset;
		std::cout << "Stride: " << offset << std::endl;
		// Round up stride
		// TODO
		// Allocate data
		unsigned int size = meshsrc->mNumVertices * attribs.stride;
		batch.geom.vertexoffset = output.allocateVertexData(size);
		batch.geom.vertexsize = size;
		// Fill data in
		float *vertices = (float*)((char*)output.vertexdata + batch.geom.vertexoffset);
		if (meshsrc->HasPositions())
		{
			float *pos = (float*)((char*)vertices + attribs.posoffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				pos[0] = meshsrc->mVertices[j].x;
				if (swapyz)
				{
					pos[1] = meshsrc->mVertices[j].z;
					pos[2] = meshsrc->mVertices[j].y;
				}
				else
				{
					pos[1] = meshsrc->mVertices[j].y;
					pos[2] = meshsrc->mVertices[j].z;
				}
				pos = (float*)((char*)pos + batch.attribs.stride);
			}
		}
		if (meshsrc->HasNormals())
		{
			float *normal = (float*)((char*)vertices + attribs.normaloffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				normal[0] = meshsrc->mNormals[j].x;
				if (swapyz)
				{
					normal[1] = meshsrc->mNormals[j].z;
					normal[2] = meshsrc->mNormals[j].y;
				}
				else
				{
					normal[1] = meshsrc->mNormals[j].y;
					normal[2] = meshsrc->mNormals[j].z;
				}
				normal = (float*)((char*)normal + batch.attribs.stride);
			}
		}
		if (meshsrc->HasTangentsAndBitangents())
		{
			float *tangent = (float*)((char*)vertices + attribs.tangentoffset);
			float *bitangent = (float*)((char*)vertices + attribs.bitangentoffset);
			for (unsigned int j = 0; j < meshsrc->mNumVertices; j++)
			{
				tangent[0] = meshsrc->mTangents[j].x;
				if (swapyz)
				{
					tangent[1] = meshsrc->mTangents[j].z;
					tangent[2] = meshsrc->mTangents[j].y;
				}
				else
				{
					tangent[1] = meshsrc->mTangents[j].y;
					tangent[2] = meshsrc->mTangents[j].z;
				}
				bitangent[0] = meshsrc->mBitangents[j].x;
				if (swapyz)
				{
					bitangent[1] = meshsrc->mBitangents[j].z;
					bitangent[2] = meshsrc->mBitangents[j].y;
				}
				else
				{
					bitangent[1] = meshsrc->mBitangents[j].y;
					bitangent[2] = meshsrc->mBitangents[j].z;
				}
				tangent = (float*)((char*)tangent + batch.attribs.stride);
				bitangent = (float*)((char*)bitangent + batch.attribs.stride);
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
				texcoord = (float*)((char*)texcoord + batch.attribs.stride);
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
				color = (float*)((char*)color + batch.attribs.stride);
			}
		}
		// Joints
		if (meshsrc->HasBones())
		{
			std::vector<unsigned int> jointcount(meshsrc->mNumVertices, 0);
			float *firstweight = (float*)((char*)vertices + attribs.jointweightoffset);
			unsigned char *firstindex = ((unsigned char*)vertices + attribs.jointoffset);
			// Clear joint info
			for (unsigned int i = 0; i < meshsrc->mNumBones; i++)
			{
				float *weight = (float*)((char*)firstweight + i * batch.attribs.stride);
				weight[0] = 0.0f;
				weight[1] = 0.0f;
				weight[2] = 0.0f;
				weight[3] = 0.0f;
				unsigned char *index = (unsigned char*)firstindex + i * batch.attribs.stride;
				index[0] = 0;
				index[1] = 0;
				index[2] = 0;
				index[3] = 0;
			}
			// Add joints
			for (unsigned int i = 0; i < meshsrc->mNumBones; i++)
			{
				aiBone *bone = meshsrc->mBones[i];
				for (unsigned int j = 0; j < bone->mNumWeights; j++)
				{
					unsigned int vertexidx = bone->mWeights[j].mVertexId;
					float weight = bone->mWeights[j].mWeight;
					if (jointcount[vertexidx] >= 4)
						continue;
					unsigned int vboffset = vertexidx * batch.attribs.stride;
					float *weightptr = (float*)((char*)firstweight + vboffset);
					unsigned char *indexptr = firstindex + vboffset;
					indexptr[jointcount[vertexidx]] = i;
					weightptr[jointcount[vertexidx]] = weight;
					jointcount[vertexidx]++;
				}
			}
		}
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
		batch.geom.indexcount = indexcount;
		// Use smaller indices if possible
		if (maxindex - minindex < 256)
			batch.geom.indextype = 1;
		else if (maxindex - minindex < 65536)
			batch.geom.indextype = 2;
		else
			batch.geom.indextype = 4;
		batch.geom.indexsize = batch.geom.indextype * batch.geom.indexcount;
		batch.geom.indexoffset = output.allocateIndexData(batch.geom.indexsize,
		                                                  batch.geom.indextype);
		batch.geom.basevertex = minindex;
		// Fill in indices
		if (batch.geom.indextype == 1)
		{
			unsigned char *indices = (unsigned char*)output.indexdata
			                       + batch.geom.indexoffset;
			for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
			{
				indices[j * 3] = meshsrc->mFaces[j].mIndices[0] - minindex;
				indices[j * 3 + 1] = meshsrc->mFaces[j].mIndices[1] - minindex;
				indices[j * 3 + 2] = meshsrc->mFaces[j].mIndices[2] - minindex;
			}
		}
		else if (batch.geom.indextype == 2)
		{
			unsigned short *indices = (unsigned short*)((char*)output.indexdata
			                        + batch.geom.indexoffset);
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
			                      + batch.geom.indexoffset);
			for (unsigned int j = 0; j < meshsrc->mNumFaces; j++)
			{
				indices[j * 3] = meshsrc->mFaces[j].mIndices[0] - minindex;
				indices[j * 3 + 1] = meshsrc->mFaces[j].mIndices[1] - minindex;
				indices[j * 3 + 2] = meshsrc->mFaces[j].mIndices[2] - minindex;
			}
		}
		// Joint bind matrices
		batch.geom.jointcount = meshsrc->mNumBones;
		if (meshsrc->mNumBones > 0)
		{
			batch.jointmatrices = std::vector<float>(meshsrc->mNumBones * 16);
			for (unsigned int j = 0; j < meshsrc->mNumBones; j++)
			{
				float *matrix = &batch.jointmatrices[j * 16];
				for (unsigned int k = 0; k < 16; k++)
				{
					matrix[k] = meshsrc->mBones[j]->mOffsetMatrix[k / 4][k % 4];
				}
			}
		}
		// Add mesh to file
		output.batches.push_back(batch);
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
		GeometryFile::Header header;
		header.tag = GeometryFile::tag;
		header.version = GeometryFile::version;
		header.vertexdatasize = output.vertexdatasize;
		header.indexdatasize = output.indexdatasize;
		header.batchcount = output.batches.size();
		file.write((char*)&header, sizeof(header));
		std::cout << "Written:" << std::endl;
		std::cout << output.vertexdatasize << " bytes vertices, ";
		std::cout << output.indexdatasize << " bytes indices, ";
		std::cout << output.batches.size() << " batches." << std::endl;
		// Write vertex/index data
		file.write((char*)output.vertexdata, output.vertexdatasize);
		file.write((char*)output.indexdata, output.indexdatasize);
		// Write mesh info
		for (unsigned int i = 0; i < output.batches.size(); i++)
		{
			GeometryFile::Batch &batch = output.batches[i];
			file.write((char*)&batch.attribs, sizeof(batch.attribs));
			file.write((char*)&batch.geom, sizeof(batch.geom));
			if (batch.geom.jointcount)
				file.write((char*)&batch.jointmatrices[0],
				           sizeof(float) * batch.geom.jointcount * 16);
		}
	}
	// Create XML model file
	TiXmlDocument xml((filename + ".model.xml").c_str());
	TiXmlDeclaration *dec = new TiXmlDeclaration("1.0", "", "");
	xml.LinkEndChild(dec);
	TiXmlElement *root = new TiXmlElement("Model");
	xml.LinkEndChild(root);
	root->SetAttribute("geometry",(relfilename + ".geo").c_str());
	// Write the armature
	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		aiMesh *mesh = scene->mMeshes[i];
		if (mesh->mNumBones > 0)
		{
			TiXmlElement *armature = new TiXmlElement("Armature");
			root->LinkEndChild(armature);
			armature->SetAttribute("batch", i);
			// Write joints
			for (unsigned int j = 0; j < mesh->mNumBones; j++)
			{
				TiXmlElement *joint = new TiXmlElement("Joint");
				armature->LinkEndChild(joint);
				joint->SetAttribute("index", j);
				joint->SetAttribute("name", mesh->mBones[j]->mName.data);
			}
		}
	}
	// Write the node tree
	writeNode(scene, scene->mRootNode, root, relfilename);
	// Save file
	xml.SaveFile();
	// Write animations
	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation *anim = scene->mAnimations[i];
		std::string animfilename = filename + "." + anim->mName.data + ".anim";
		if (!strcmp(anim->mName.data, ""))
			animfilename = filename + ".anim";
		std::ofstream file(animfilename.c_str());
		if (!file)
		{
			std::cerr << "Could not open " << animfilename + ".geo" << std::endl;
			return -1;
		}
		// Get animation length
		unsigned int framecount = (unsigned int)anim->mDuration;
		// Write animation header
		AnimationFile::Header header;
		header.tag = AnimationFile::tag;
		header.version = AnimationFile::version;
		header.channelcount = anim->mNumChannels;
		header.framecount = framecount;
		header.framespersecond = anim->mTicksPerSecond;
		file.write((char*)&header, sizeof(header));
		// Write channels
		for (unsigned int i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim *channel = anim->mChannels[i];
			// Channel header
			AnimationFile::Channel channelhdr;
			memset(&channelhdr, 0, sizeof(channelhdr));
			strncpy(channelhdr.name,
			        channel->mNodeName.data,
			        AnimationFile::maxnamesize);
			channelhdr.constant = 0;
			// Write frames
			AnimationFile::Frame *frames = new AnimationFile::Frame[framecount];
			for (unsigned int i = 0; i < framecount; i++)
			{
				frames[i] = getAnimationFrame(channel, i);
			}
			file.write((char*)frames, sizeof(AnimationFile::Frame) * framecount);
			delete[] frames;
		}
	}
	return 0;
}
