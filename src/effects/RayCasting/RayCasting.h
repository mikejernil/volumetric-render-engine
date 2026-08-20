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

// Shader Type 3:Pseudo IsoSurface with Ray Casting Method :  global variables:
extern GLuint shaderProgramObject_IsoSurface;
extern GLuint modelViewProjectionUniform_IsoSurface;
extern GLuint textureVolumeUniform_IsoSurface;
extern GLuint cameraPositionUniform_IsoSurface;
extern GLuint step_size_Uniform_IsoSurface;


// function declarations:


int Initialize_Raycasting_shader(void);
void Initialize_Raycasting_Geomatry(void);
void Render_Raycasting_Output(void);
void Update_Raycasting_Output(void);
void Update_Raycasting_Cube_VBO(void);
void Uninitialize_Raycasting_shader(void);

// Shader Type 3:Pseudo IsoSurface with Ray Casting Method : prototypes:
int Initialize_IsoSurface_shader(void);
void Initialize_IsoSurface_Geomatry(void);
void Render_IsoSurface_Output(void);
void Update_IsoSurface_Output(void);
void Uninitialize_IsoSurface_shader(void);

