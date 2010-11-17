
uniform mat4 worldMat;
uniform mat4 worldNormalMat;
uniform mat4 viewProjMat;
uniform mat4 viewMat;
uniform mat4 viewProjMatInv;

uniform vec4 lightPos;

#if Skinning
	attribute vec4 jointindex;
	attribute vec4 jointweight;
	uniform mat4 skinMat[50];
#endif

#if Instancing
	attribute mat4 transMat;
#endif
