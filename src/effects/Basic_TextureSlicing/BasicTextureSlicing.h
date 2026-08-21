#pragma once

#include "../../utils/common.h"
#include "../../utils/OpenGL/GLShaders.h"


// variable declarations:
extern GLuint shaderProgramObject_Slicer1;
extern GLuint modelViewProjectionUniform_Slicer1;
extern GLuint textureVolumeUniform_Slicer1;

extern GLuint VBO_volume;
extern GLuint VAO_volume;

// Shader Type 4: Colomap Classifaction Method : global variables:
extern GLuint shaderProgramObject_Colormap;
extern GLuint modelViewProjectionUniform_Colormap;
extern GLuint textureVolumeUniform_Colormap;
extern GLuint levelOfDetail_Uniform;
extern GLuint texture_TransferFunction;

extern glm::vec4 jet_values[9];


// function declarations:


int Initialize_Slicing_shader(void);
void Initialize_Slicing_Geometry(void);
void Slice_Volume(void);
int FindAbsMax(glm::vec3 v);
int LoadVolumeData(void);
void Render_Basic_Volume(void);
void Update_Basic_Volume(void);
void Uninitialize_Slicing_shader(void);

// Shader Type 4: Colomap Classifaction Method : prototypes:
int Initialize_ColormapClassification_shader(void);
void LoadTransferFunction(void);
void Initialize_ColormapClassification_Geomatry(void);
void Render_ColormapClassification_Output(void);
void Update_ColormapClassification_Output(void);
void Uninitialize_ColormapClassification_shader(void);

