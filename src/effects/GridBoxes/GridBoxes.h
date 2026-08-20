#pragma once

#include "../../utils/common.h"


// variable declarations:

extern GLuint shaderProgramObject_Grid;
extern GLuint mvpUniform_GridObject;

extern glm::vec3 vertexList[8];


extern GLuint VAO_Volume_Axes;
extern GLuint VBO_Volume_Axes;


// function declarations:

int LoadGridObject_Shader(int width, int depth);
void setup_Axes_BufferObjects(void);

void Update_Volume_Box_Axes(void);
void Render_Volume_Box_Axes(glm::mat4);

void Uninitialize_Grid(void);

