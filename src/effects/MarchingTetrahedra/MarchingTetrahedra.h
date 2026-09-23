#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLShaders.h"




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

extern GLuint volumeMarcherVBO_1;
extern GLuint volumeMarcherVAO_1;

extern GLuint volumeMarcherVBO_2;
extern GLuint volumeMarcherVAO_2;

extern GLuint volumeMarcherVBO_3;
extern GLuint volumeMarcherVAO_3;

extern GLuint volumeMarcherVBO_4;
extern GLuint volumeMarcherVAO_4;

extern GLubyte* pVolume_1;
extern GLubyte* pVolume_2;
extern GLubyte* pVolume_3;
extern GLubyte* pVolume_4;

extern BOOL bRecalculateForIsoValue;

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
void Initialize_TetrahedraMarcher_Geomatry(GLuint *,GLuint *);
void SetVolumeDimensions(const int xdim, const int ydim, const int zdim);//function to set the volume dimension
void SetNumSamplingVoxels(const int x, const int y, const int z);//function to set the total number of sampling voxels + more voxels will give a higher density mesh
void SetIsosurfaceValue(const GLubyte value);//set the isosurface value
bool LoadVolume_MT(const std::string , GLubyte** );
bool Load_Volume_Data_MT_Custom_Rotate(const std::string , GLubyte** );
void MarchVolume(GLubyte*);//march the volume dataset
size_t GetTotalVertices_TM();//get the total number of vertices generated
Vertex* GetVertexPointer_TM();//get the pointer to the vertex buffer
GLubyte SampleVolume(const int x, const int y, const int z, GLubyte*);
glm::vec3 GetNormal(const int x, const int y, const int z, GLubyte*);//get the normal at the given location using center finite difference approximation
void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale, GLubyte*);//samples a voxel at the given location and scale
float GetOffset(const GLubyte v1, const GLubyte v2);//returns the offset between the two sample values
void Render_MarchingTetrahedra(void);

void ReCalculate_VAO(GLuint , GLuint , GLubyte* );
void Update_MarchingTetrahedra(void);
void Uninitialize_MarchingTetrahedra(void);
