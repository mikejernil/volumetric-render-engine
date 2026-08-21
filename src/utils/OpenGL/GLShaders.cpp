

#include <io.h>

#include "GLShaders.h"

GLuint CreateAndCompileShaderObjects(const char *shaderFilename, GLShaderType shaderType)
{
	// variable declarations
	GLuint shaderObject = 0;
	char shaderNameTag[50];
	GLenum glShaderType;

	int fdShaderFile = -1;
	long shaderFileSize = -1;
	char *shaderSourceCodeBuffer = NULL;

	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	fdShaderFile = _open(shaderFilename, _O_RDONLY, _S_IREAD);
	if (fdShaderFile == -1)
	{
		PrintLog("ERROR: Failed To Open Shader File %s.\nClosing the program.\n", shaderFilename);
		return(0);
	}

	shaderFileSize = _lseek(fdShaderFile, 0, SEEK_END);
	shaderFileSize = shaderFileSize + 1;
	shaderSourceCodeBuffer = (char *)malloc(shaderFileSize);
	_lseek(fdShaderFile, 0, SEEK_SET);

	shaderFileSize = _read(fdShaderFile, (char *)shaderSourceCodeBuffer, shaderFileSize);
	if (shaderFileSize < 0)
	{
		PrintLog("ERROR: Failed To Read Shader File %s.\nClosing the program.\n", shaderFilename);
		return(0);
	}
	
	shaderSourceCodeBuffer[shaderFileSize] = '\0';
	_close(fdShaderFile);

	switch (shaderType)
	{
	case VERTEX:
		strcpy_s(shaderNameTag, 50, "Vertex");
		glShaderType = GL_VERTEX_SHADER;
		break;

	case FRAGMENT:
		strcpy_s(shaderNameTag, 50, "Fragment");
		glShaderType = GL_FRAGMENT_SHADER;
		break;

	default:
		PrintLog("ERROR: Invalid Shader Type.\nClosing the program.\n");
		return(0);
	}

	// CREATING SHADER OBJECT
	shaderObject = glCreateShader(glShaderType);
	if (shaderObject == 0)
	{
		PrintLog("ERROR: for \"%s\" : Failed To Create %s Shader Object.\nClosing the program.\n", shaderFilename, shaderNameTag);
		return(0);
	}

	glShaderSource(shaderObject, 1, (const GLchar **)&shaderSourceCodeBuffer, NULL);
	free(shaderSourceCodeBuffer);

	glCompileShader(shaderObject);

	// Step 5

	// 5(a)
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		PrintLog("ERROR: for \"%s\" : %s Shader Compilation Failed.\n", shaderFilename, shaderNameTag);
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(sizeof(char) * infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(shaderObject, infoLogLength, &written, log);
				PrintLog("ERROR: %s Shader Compilation Log : %s\n",shaderNameTag, log);
				free(log);
				return(0);
			}
		}
	}

	else
	{
		PrintLog("SUCCESS: \"%s\" : %s Shader Compiled Successfully.\n", shaderFilename, shaderNameTag);
	}

	return(shaderObject);
}

BOOL LinkShaderProgramObject(GLuint shaderProgramObject)
{
	// variable declarations
	int status = 0;
	int infoLogLength = 0;
	char *log = NULL;

	// code
	glLinkProgram(shaderProgramObject);

	// Step 4
	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		PrintLog("ERROR: Shader Program Linking Failed.\n");
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			log = (char *)malloc(sizeof(char) * infoLogLength);
			if (log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, &written, log);
				PrintLog("ERROR: Shader Program Linking Log : %s\n", log);
				free(log);
				return(FALSE);
			}
		}
	}

	else
	{
		PrintLog("SUCCESS: Shader Program Linking Successful.\n");
	}

	return(TRUE);
}

void UninitializeShaders(GLuint shaderProgramObject)
{
	static int iCount = 1;
	// code
	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLsizei numAttachedShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numAttachedShaders);

		GLuint *shaderObjects = NULL;
		shaderObjects = (GLuint *)malloc(sizeof(GLuint) * numAttachedShaders);

		glGetAttachedShaders(shaderProgramObject, numAttachedShaders, &numAttachedShaders, shaderObjects);

		for (GLsizei i = 0; i < numAttachedShaders; i++)
		{
			glDetachShader(shaderProgramObject, shaderObjects[i]);
			glDeleteShader(shaderObjects[i]);
			shaderObjects[i] = 0;
		}

		free(shaderObjects);
		shaderObjects = NULL;

		glUseProgram(0);

		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
		PrintLog("SUCCES: after UninitializeShaders() time : %d \n", iCount);
		iCount++;
	}
}

