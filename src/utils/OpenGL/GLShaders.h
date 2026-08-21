#pragma once

#include "GLHeadersAndMacros.h"
#include "GLLog.h"

#include <fcntl.h> // For _O_RDONLY
#include <sys/stat.h> // For _S_IREAD

GLuint CreateAndCompileShaderObjects(const char *shaderFilename, GLShaderType shaderType);
BOOL LinkShaderProgramObject(GLuint shaderProgramObject);
void UninitializeShaders(GLuint shaderProgramObject);



