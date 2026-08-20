#pragma once

#include "../../utils/common.h"


// variable declarations:
extern GLuint shaderProgramObject_Slicer1;
extern GLuint modelViewProjectionUniform_Slicer1;
extern GLuint textureVolumeUniform_Slicer1;

extern GLuint VBO_volume;
extern GLuint VAO_volume;
extern GLuint textureID;

// function declarations:



int Initialize_Slicing_shader(void);
void Slice_Volume(void);
int FindAbsMax(glm::vec3 v);
int LoadVolumeData(void);
void Render_Basic_Volume(void);
void Update_Basic_Volume(void);
void Uninitialize_Slicing_shader(void);

