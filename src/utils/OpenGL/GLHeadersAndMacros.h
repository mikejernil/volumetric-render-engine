#pragma once

// GLEW
#include "../../../dependencies/glew/include/GL/glew.h"
// OpenGL Header File
#include <GL/gl.h>

#include "../../include/glm/glm.hpp"
#include "../../include/glm/gtc/matrix_transform.hpp"
#include "../../include/glm/gtc/type_ptr.hpp"

// GLEW
#pragma comment(lib,"glew32")
// OpenGL Libraries
#pragma comment(lib, "OpenGL32")

// 

enum
{
	ATTRIBUTE_POSITION = 0,
	ATTRIBUTE_COLOR,
	ATTRIBUTE_NORMAL,
	ATTRIBUTE_TEXTURE
};

enum GLShaderType
{
	VERTEX = 0,
	FRAGMENT,
};
