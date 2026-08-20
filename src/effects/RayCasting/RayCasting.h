#pragma once

#include "../../utils/common.h"


// variable declarations:

extern GLuint shaderProgramObject_RayCasting;
extern GLuint modelViewProjectionUniform_RayCasting;
extern GLuint textureVolumeUniform_RayCasting;
extern GLuint cameraPositionUniform;
extern GLuint step_size_Uniform;

extern GLuint VAO_cube_RayCastingCube;
extern GLuint VBO_cube_ID_RayCastingCube;
extern GLuint VBO_cube_Indices_RayCastingCube;

// function declarations:



int Initialize_Raycasting_shader(void);
void Initialize_Raycasting_Geomatry(void);
void Render_Raycasting_Output(void);
void Update_Raycasting_Output(void);
void Update_Raycasting_Cube_VBO(void);
void Uninitialize_Raycasting_shader(void);


