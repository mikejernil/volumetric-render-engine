#pragma once

#include "./OpenGL/GLHeadersAndMacros.h"
#include "./OpenGL/GLLog.h"

//#include "../include/vmath.h"
#include "../include/stb_image.h"


#define EPSILON 0.0001f;//for floating point inaccuracy

//using namespace vmath;


// variable declarations:

extern int MAX_SLICES;
extern int XDIM;
extern int YDIM;
extern int ZDIM;

extern GLuint textureID;

extern float fZPlus_FrontFace;
extern float fYPlus_TopFace;
extern float fXPlus_SideFace;
extern float fXMinus_SideFace;
extern float fYMinus_BottomFace;
extern float fZMinus_BackFace;


extern glm::vec3 viewDirection;

extern int num_slices;

//unit cube edges
extern int edgeList[8][12];

extern int edges[12][2];

extern glm::vec3 vTextureSlices[512 * 12];

extern float dist;

extern float rotationX;
extern float rotationY;
extern float rotationZ;

extern glm::mat4 perspectiveProjMatrix_glm;

// function declarations:
float degreeToRadians(float);


