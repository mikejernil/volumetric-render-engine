#pragma once

#include "RayCasting.h"




GLuint shaderProgramObject_RayCasting = 0;
GLuint modelViewProjectionUniform_RayCasting = 0;
GLuint textureVolumeUniform_RayCasting = 0;
GLuint cameraPositionUniform = 0;
GLuint step_size_Uniform = 0;

GLuint VAO_cube_RayCastingCube =0;
GLuint VBO_cube_ID_RayCastingCube =0;
GLuint VBO_cube_Indices_RayCastingCube = 0;

// Shader Type 3:Pseudo IsoSurface with Ray Casting Method :  global variables:
GLuint shaderProgramObject_IsoSurface = 0;
GLuint modelViewProjectionUniform_IsoSurface = 0;
GLuint textureVolumeUniform_IsoSurface = 0;
GLuint cameraPositionUniform_IsoSurface = 0;
GLuint step_size_Uniform_IsoSurface = 0;


// function definitions:

//! Shader Type 2: RAY CASTING Method Definitions:

int Initialize_Raycasting_shader(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;


	// code:

	vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\RayCasting\\RayCasting.vs.glsl", VERTEX);
	fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\RayCasting\\RayCasting.fs.glsl", FRAGMENT);


	shaderProgramObject_RayCasting = glCreateProgram();


	glAttachShader(shaderProgramObject_RayCasting, vertexShaderObject);
	glAttachShader(shaderProgramObject_RayCasting, fragmentShaderObject);


	if (LinkShaderProgramObject(shaderProgramObject_RayCasting) == FALSE)
	{
		PrintLog("shaderProgramObject_RayCasting Linking FAILED \n");
	}
	else
	{
		PrintLog("shaderProgramObject_RayCasting Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_RayCasting);

	glBindAttribLocation(shaderProgramObject_RayCasting, ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_RayCasting = glGetUniformLocation(shaderProgramObject_RayCasting, "u_MVPMatrix");
	textureVolumeUniform_RayCasting = glGetUniformLocation(shaderProgramObject_RayCasting, "u_Volume3DSampler");
	cameraPositionUniform = glGetUniformLocation(shaderProgramObject_RayCasting, "u_cameraPosition");
	step_size_Uniform = glGetUniformLocation(shaderProgramObject_RayCasting, "u_stepSize");

	glUniform1i(textureVolumeUniform_RayCasting, 0);

	glUseProgram(0);



	return (0);
}

void Initialize_Raycasting_Geomatry(void)
{
	// code:

	glm::vec3 vertices[8] = { glm::vec3(-0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f,-0.5f,-0.5f),
							glm::vec3(0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f, 0.5f,-0.5f),
							glm::vec3(-0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f,-0.5f, 0.5f),
							glm::vec3(0.5f, 0.5f, 0.5f),
							glm::vec3(-0.5f, 0.5f, 0.5f) };

	//unit cube indices
	GLushort cubeIndices[36] = { 0,5,4,
							  5,0,1,
							  3,7,6,
							  3,6,2,
							  7,4,6,
							  6,4,5,
							  2,1,3,
							  3,1,0,
							  3,0,7,
							  7,0,4,
							  6,5,2,
							  2,5,1 };


	glGenVertexArrays(1, &VAO_cube_RayCastingCube);
	glBindVertexArray(VAO_cube_RayCastingCube);
	{
		// VBO_cube_ID : //pass cube vertices to buffer object memory
		glGenBuffers(1, &VBO_cube_ID_RayCastingCube);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_cube_ID_RayCastingCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO_cube_Indices : //pass indices to element array  buffer
		glGenBuffers(1, &VBO_cube_Indices_RayCastingCube);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_cube_Indices_RayCastingCube);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);


}

void Update_Raycasting_Cube_VBO(void)
{
	// code:

	glm::vec3 vertices[8] =
	{
		// Minus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,fZMinus_BackFace), // 1. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),  // 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),  // 3. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace), // 4. Left Top

		// Plus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace), // 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),  // 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace)  // 8. Left Top
	};

	//unit cube indices
	GLushort cubeIndices[36] =
	{
		0,5,4,
		5,0,1,
		3,7,6,
		3,6,2,
		7,4,6,
		6,4,5,
		2,1,3,
		3,1,0,
		3,0,7,
		7,0,4,
		6,5,2,
		2,5,1
	};

	glBindVertexArray(VAO_cube_RayCastingCube);
	{
		// VBO_cube_ID : //pass cube vertices to buffer object memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO_cube_ID_RayCastingCube);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &(vertices[0].x), GL_DYNAMIC_DRAW);
		//glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		// VBO_cube_Indices : //pass indices to element array  buffer
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBO_cube_Indices_RayCastingCube);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), &cubeIndices[0], GL_DYNAMIC_DRAW);
		//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

}

void Render_Raycasting_Output(void)
{
	// local:

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));


	// code:

	glEnable(GL_BLEND);
	glUseProgram(shaderProgramObject_RayCasting);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_RayCasting, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glUniform3fv(cameraPositionUniform, 1, &(cameraPosition.x));
		glUniform3f(step_size_Uniform, (1.0f / XDIM), (1.0f / YDIM), (1.0f / ZDIM));

		// Texture : 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_RayCasting, 0);


		glBindVertexArray(VAO_cube_RayCastingCube);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

}

void Update_Raycasting_Output(void)
{
	// local:

	// code;

}

void Uninitialize_Raycasting_shader(void)
{

	// code:

	if (VBO_cube_Indices_RayCastingCube)
	{
		glDeleteBuffers(1, &VBO_cube_Indices_RayCastingCube);
		VBO_cube_Indices_RayCastingCube = 0;
	}
	if (VBO_cube_ID_RayCastingCube)
	{
		glDeleteBuffers(1, &VBO_cube_ID_RayCastingCube);
		VBO_cube_ID_RayCastingCube = 0;
	}

	if (VAO_cube_RayCastingCube)
	{
		glDeleteVertexArrays(1, &VAO_cube_RayCastingCube);
		VAO_cube_RayCastingCube = 0;
	}

	UninitializeShaders(shaderProgramObject_RayCasting);

}



//! Shader Type 3: Iso Surface with RAY CASTING Method Definitions:

int Initialize_IsoSurface_shader(void)
{
	// prototype:
	void uninitialize(void);

	// local:


	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	GLint status;
	GLint infoLogLength;
	char* Log = NULL;

	// code:


	vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\IsoSurface\\IsoSurface.vs.glsl", VERTEX);
	fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\IsoSurface\\IsoSurface.fs.glsl", FRAGMENT);


	shaderProgramObject_IsoSurface = glCreateProgram();


	glAttachShader(shaderProgramObject_IsoSurface, vertexShaderObject);
	glAttachShader(shaderProgramObject_IsoSurface, fragmentShaderObject);

	if (LinkShaderProgramObject(shaderProgramObject_IsoSurface) == FALSE)
	{
		PrintLog("shaderProgramObject_IsoSurface Linking FAILED \n");
	}
	else
	{
		PrintLog("shaderProgramObject_IsoSurface Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_IsoSurface);

	glBindAttribLocation(shaderProgramObject_IsoSurface, ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_MVPMatrix");
	textureVolumeUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_Volume3DSampler");
	cameraPositionUniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_cameraPosition");
	step_size_Uniform_IsoSurface = glGetUniformLocation(shaderProgramObject_IsoSurface, "u_stepSize");

	glUniform1i(textureVolumeUniform_IsoSurface, 0);

	glUseProgram(0);



	return (0);
}

void Initialize_IsoSurface_Geomatry(void)
{
	// local:

	// code:

}

void Render_IsoSurface_Output(void)
{
	// local:
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));

	// code:



	glEnable(GL_BLEND);
	glUseProgram(shaderProgramObject_IsoSurface);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_IsoSurface, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));//pass the shader uniform
		glUniform3fv(cameraPositionUniform_IsoSurface, 1, &(cameraPosition.x));
		glUniform3f(step_size_Uniform_IsoSurface, (1.0f / XDIM), (1.0f / YDIM), (1.0f / ZDIM));

		// Texture : 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_IsoSurface, 0);

		glBindVertexArray(VAO_cube_RayCastingCube);
		glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_3D, 0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);

}

void Update_IsoSurface_Output(void)
{
	// local:

	// code:

}

void Uninitialize_IsoSurface_shader(void)
{
	// local:

	// code:

	UninitializeShaders(shaderProgramObject_IsoSurface);

}



