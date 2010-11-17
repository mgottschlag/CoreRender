
#include "defaultUniforms.glsl"

vec4 getSkinningPos(vec4 pos)
{
#if Skinning
	// Skinning
	mat4 mat = mat4(0.0);
	for (int i = 0; i < 4; i++)
	{
		mat += skinMat[int(jointindex[i])] * jointweight[i];
	}
	return mat * pos;
#else
	return pos;
#endif
}

vec4 getWorldPos(vec4 pos)
{
#if Instancing
	return transMat * pos;
#else
	return worldMat * pos;
#endif
}
