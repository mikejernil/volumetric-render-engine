#pragma once

#include "../../utils/common.h"





// global variables for Marching Tetrahedra Method:


struct Vertex
{
	glm::vec3 pos, normal;  //our vertex struct stores the position and normals
};

extern std::vector<Vertex> vertices;

extern int XDIM_TM;
extern int YDIM_TM;
extern int ZDIM_TM;

extern GLuint shaderProgramObject_TM;
extern GLuint modelViewProjectionUniform_TM;
extern glm::vec3 invDim;

extern int X_SAMPLING_DIST;
extern int Y_SAMPLING_DIST;
extern int Z_SAMPLING_DIST;

//volume data pointer
extern GLubyte* pVolume;
extern GLubyte isoValue;

//vertices vector storing positions and normals
extern GLuint volumeMarcherVBO;
extern GLuint volumeMarcherVAO;


// clipping plane members:
extern GLuint clipFrontFace_uniform;
extern float fClipPlane_Front;

extern GLuint clipBackFace_uniform;
extern float fClipPlane_Back;

extern GLuint clipRight_uniform;
extern float fClipPlane_Right;

extern GLuint clipLeft_uniform;
extern float fClipPlane_Left;

extern GLuint clipTop_uniform;
extern float fClipPlane_Top;

extern GLuint clipBottom_uniform;
extern float fClipPlane_Bottom;


// function declarations for Marching Tetrahedra Method:

void Initialize_TetrahedraMarcher_Constructor(void);
void Initialize_TetrahedraMarcher_Shaders(void);
void Initialize_TetrahedraMarcher_Geomatry(void);
void SetVolumeDimensions(const int xdim, const int ydim, const int zdim);//function to set the volume dimension
void SetNumSamplingVoxels(const int x, const int y, const int z);//function to set the total number of sampling voxels + more voxels will give a higher density mesh
void SetIsosurfaceValue(const GLubyte value);//set the isosurface value
bool LoadVolume_MT(void);//load the volume dataset

void MarchVolume();//march the volume dataset
size_t GetTotalVertices_TM();//get the total number of vertices generated
Vertex* GetVertexPointer_TM();//get the pointer to the vertex buffer
GLubyte SampleVolume(const int x, const int y, const int z);
glm::vec3 GetNormal(const int x, const int y, const int z);//get the normal at the given location using center finite difference approximation
void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale);//samples a voxel at the given location and scale
float GetOffset(const GLubyte v1, const GLubyte v2);//returns the offset between the two sample values
void Render_MarchingTetrahedra(void);
void Update_MarchingTetrahedra(void);
void Uninitialize_MarchingTetrahedra(void);
