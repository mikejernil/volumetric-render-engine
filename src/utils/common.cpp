#include "common.h"



int MAX_SLICES = 512;
int XDIM = 256;
int YDIM = 256;
int ZDIM = 256;

GLuint textureID = 0;

float fZPlus_FrontFace = 0.5f;
float fYPlus_TopFace = 0.5f;
float fXPlus_SideFace = 0.5f;

float fXMinus_SideFace = -0.5f;
float fYMinus_BottomFace = -0.5f;
float fZMinus_BackFace = -0.5f;

BOOL bWireframe = FALSE;

glm::vec3 viewDirection = glm::vec3(1.0f);



int num_slices = 256;
//unit cube edges
int edgeList[8][12] = {
	{ 0,1,5,6,   4,8,11,9,  3,7,2,10 }, // v0 is front
	{ 0,4,3,11,  1,2,6,7,   5,9,8,10 }, // v1 is 
	{ 1,5,0,8,   2,3,7,4,   6,10,9,11}, // v2 is 
	{ 7,11,10,8, 2,6,1,9,   3,0,4,5  }, // v3 is 
	{ 8,5,9,1,   11,10,7,6, 4,3,0,2  }, // v4 is 
	{ 9,6,10,2,  8,11,4,7,  5,0,1,3  }, // v5 is 
	{ 9,8,5,4,   6,1,2,0,   10,7,11,3}, // v6 is 
	{ 10,9,6,5,  7,2,3,1,   11,4,8,0 }  // v7 is 
};
int edges[12][2] = { {0,1},{1,2},{2,3},{3,0},{0,4},{1,5},{2,6},{3,7},{4,5},{5,6},{6,7},{7,4} };

glm::vec3 vTextureSlices[512 * 12];

float dist = -2.0f;


float rotationX = 0.0f;
float rotationY = 0.0f;
float rotationZ = 0.0f;

glm::mat4 perspectiveProjMatrix_glm = glm::identity<glm::mat4>();

