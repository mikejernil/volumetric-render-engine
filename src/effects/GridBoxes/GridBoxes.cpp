#pragma once

#include "GridBoxes.h"


// variable declarations:


GLuint shaderProgramObject_Grid = 0;
GLuint mvpUniform_GridObject = 0;

glm::vec3 vertexList[8] = {
	// Minus Z-Vertices
	glm::vec3(-0.5,-0.5,-0.5), // Left Bottom // 0th
	glm::vec3(0.5,-0.5,-0.5),  // Right Bottom // 1st
	glm::vec3(0.5, 0.5,-0.5),  // Right Top
	glm::vec3(-0.5, 0.5,-0.5), // Left Top

	// Plus Z-Vertices
	glm::vec3(-0.5,-0.5, 0.5), // Left Bottom
	glm::vec3(0.5,-0.5, 0.5),  // Right Bottom
	glm::vec3(0.5, 0.5, 0.5),  // Right Top
	glm::vec3(-0.5, 0.5, 0.5)  // Left Top
};



GLuint VAO_Volume_Axes = 0;
GLuint VBO_Volume_Axes = 0;



// function definitions:

int LoadGridObject_Shader(int width, int depth)
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

	///* //////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			void main()
			{
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);
			}
	
			)";

	vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSource, NULL);

	glCompileShader(vertexShaderObject);
	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, &written, Log);
				PrintLog(" GridObject Vertex Shader Compilation Log : %s\n", Log);

				free(Log);
				Log = NULL;
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		PrintLog("GridObject Success at  Vertex Shader Compilation \n");
	}



	//* ///////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			uniform vec4 u_Color;
			out vec4 FragColor;		
			void main(void)
			{             
				FragColor = u_Color ;
			}

			)";
	fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSource, NULL);

	glCompileShader(fragmentShaderObject);
	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, &written, Log);
				PrintLog("GridObject  FRAGMENT Shader Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		PrintLog("GridObject Success at  FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_Grid = glCreateProgram();


	glAttachShader(shaderProgramObject_Grid, vertexShaderObject);
	glAttachShader(shaderProgramObject_Grid, fragmentShaderObject);


	glBindAttribLocation(shaderProgramObject_Grid, ATTRIBUTE_POSITION, "aPosition");

	infoLogLength = 0;
	Log = NULL;
	glLinkProgram(shaderProgramObject_Grid);
	glGetProgramiv(shaderProgramObject_Grid, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_Grid, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);
			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_Grid, infoLogLength, &written, Log);
				PrintLog("GridObject  Shader Compilation Log : %s\n", Log);
				free(Log);
				uninitialize();
				return -1;
			}
		}
	}
	else
	{
		PrintLog("GridObject  Linking Successful \n");
	}

	mvpUniform_GridObject = glGetUniformLocation(shaderProgramObject_Grid, "u_MVPMatrix");


	return 0;
}

void setup_Axes_BufferObjects(void)
{
	// code:

	glGenVertexArrays(1, &VAO_Volume_Axes);
	glBindVertexArray(VAO_Volume_Axes);
	{
		glm::vec3 vertexList[24] = {
			// FRONT FACE: All Plus Z-Vertices
			glm::vec3(-0.5f,-0.5f, fZPlus_FrontFace), // 5. Left Bottom
			glm::vec3(fXPlus_SideFace,-0.5f, fZPlus_FrontFace),  // 6. Right Bottom
			glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
			glm::vec3(-0.5f, fYPlus_TopFace, fZPlus_FrontFace),  // 8. Left Top

			// RIGHT SIDE FACE : All Plus X-Vertices:
			glm::vec3(fXPlus_SideFace, 0.5f, 0.5f),
			glm::vec3(fXPlus_SideFace, 0.5f,-0.5f),
			glm::vec3(fXPlus_SideFace,-0.5f,-0.5f),
			glm::vec3(fXPlus_SideFace,-0.5f, 0.5f),

			// TOP FACE : All Plus Y-Vertices:
			glm::vec3(-0.5f, fYPlus_TopFace,-0.5f),
			glm::vec3(0.5f, fYPlus_TopFace,-0.5f),
			glm::vec3(0.5f, fYPlus_TopFace, 0.5f),
			glm::vec3(-0.5f, fYPlus_TopFace, 0.5f),

			// BOTTOM FACE : All Minus  Y-Vertices:
			glm::vec3(-0.5f, fYMinus_BottomFace,-0.5f),
			glm::vec3(0.5f, fYMinus_BottomFace,-0.5f),
			glm::vec3(0.5f, fYMinus_BottomFace, 0.5f),
			glm::vec3(-0.5f, fYMinus_BottomFace, 0.5f),

			// BACK FACE: All Minus Z-Vertices
			glm::vec3(-0.5f,-0.5f, fZMinus_BackFace),
			glm::vec3(0.5f,-0.5f, fZMinus_BackFace),
			glm::vec3(0.5f,  0.5f, fZMinus_BackFace),
			glm::vec3(-0.5f,  0.5f, fZMinus_BackFace),

			// LEFT SIDE FACE: All Minus X-Vertices
			glm::vec3(fXMinus_SideFace, 0.5f, 0.5f),
			glm::vec3(fXMinus_SideFace, 0.5f,-0.5f),
			glm::vec3(fXMinus_SideFace,-0.5f,-0.5f),
			glm::vec3(fXMinus_SideFace,-0.5f, 0.5f),

		};

		glGenBuffers(1, &VBO_Volume_Axes);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_Volume_Axes);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertexList), NULL, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);
}

void Render_Volume_Box_Axes(glm::mat4 MVPMatrix_)
{
	// code:

	glUseProgram(shaderProgramObject_Grid);
	{
		// Front Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 0.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 0, 4);
		glBindVertexArray(0);

		// Right Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 4, 4);
		glBindVertexArray(0);

		// Top Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 0.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 8, 4);
		glBindVertexArray(0);

		// Bottom Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 1.0f, 0.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 12, 4);
		glBindVertexArray(0);

		// Left Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 0.0f, 1.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 16, 4);
		glBindVertexArray(0);

		// Back Face
		glUniformMatrix4fv(mvpUniform_GridObject, 1, GL_FALSE, glm::value_ptr(MVPMatrix_));
		glUniform4f(glGetUniformLocation(shaderProgramObject_Grid, "u_Color"), 1.0f, 0.0f, 1.0f, 1.0f);
		glBindVertexArray(VAO_Volume_Axes);
		glDrawArrays(GL_LINE_LOOP, 20, 4);
		glBindVertexArray(0);
	}
	glUseProgram(0);


}

void Update_Volume_Box_Axes(void)
{
	// code:

	glm::vec3 vertexList[24] = {

		// FRONT FACE: All Plus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),				// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),	// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),		// 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),					// 8. Left Top

		// RIGHT FACE : All Plus X-Vertices:
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),

		// TOP FACE: All Plus Y-Vertices
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),

		// BOTTOM FACE: All Minus Y-Vertices
		glm::vec3(fXMinus_SideFace, fYMinus_BottomFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace, fYMinus_BottomFace,fZMinus_BackFace),
		glm::vec3(fXPlus_SideFace, fYMinus_BottomFace, fZPlus_FrontFace),
		glm::vec3(fXMinus_SideFace, fYMinus_BottomFace, fZPlus_FrontFace),

		// LEFT FACE : All Minus X-Vertices:
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace),
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),

		// BACK FACE: All Minus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZMinus_BackFace),				// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZMinus_BackFace),	// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZMinus_BackFace),		// 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZMinus_BackFace),					// 8. Left Top

	};


	glBindBuffer(GL_ARRAY_BUFFER, VBO_Volume_Axes);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexList), &(vertexList[0].x), GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void Uninitialize_Grid(void)
{
	// code:
	if (VBO_Volume_Axes)
	{
		glDeleteBuffers(1, &VBO_Volume_Axes);
		VBO_Volume_Axes = 0;
	}
	if (VAO_Volume_Axes)
	{
		glDeleteVertexArrays(1, &VAO_Volume_Axes);
		VAO_Volume_Axes = 0;
	}

	UninitializeShaders(shaderProgramObject_Grid);

}


