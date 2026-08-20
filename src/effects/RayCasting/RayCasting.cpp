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




// function definitions:



int Initialize_Raycasting_shader(void)
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

	/////////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords;
			void main()
			{
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);

				oTexCoords = aPosition + vec3(0.5);
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
				PrintLog("Error in Raycating Vertex Shader .\n VS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		PrintLog("Success in Raycating  Vertex Shader Compilation \n");
	}



	/////////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oTexCoords;

			uniform sampler3D u_Volume3DSampler;
			uniform vec3 u_cameraPosition;
			uniform vec3 u_stepSize;

			const int MAX_SAMPLES = 300;
			const vec3 texMin = vec3(0);
			const vec3 texMax = vec3(1);

			out vec4 FragColor;		
			void main(void)
			{             

				// step 1:
				vec3 dataPosition = oTexCoords; 

				// step 2: 
				/*
				get the object space position by subracting 0.5 from the 3D texture coordinates.
				Then subtraact it from camera position and normalize to get the ray marching direction
				*/

				vec3 geomatryDirection = normalize((oTexCoords - vec3(0.5)) - u_cameraPosition);


				// step 3: multiply the raymarching direction with the step size to get the sub-step size we need to take at each raymarching step
				vec3 directionStep = geomatryDirection * u_stepSize;

				//flag to indicate if the raymarch loop should terminate
				bool bStop = false;

				for(int i = 0; i<MAX_SAMPLES; i++)
				{
					dataPosition = dataPosition + directionStep;

					bStop = dot(sign(dataPosition - texMin),sign(texMax - dataPosition)) < 3.0;

					if(bStop)
						break;

					// data fetching from the Red Channel of Volume Texture:
					float fSample = texture(u_Volume3DSampler,dataPosition).r;


					float prev_alpha = fSample - (fSample * FragColor.a);
					FragColor.rgb = prev_alpha * vec3(fSample) + FragColor.rgb; 
					FragColor.a += prev_alpha; 

					//Ray Termination : if the currently composited colour alpha is already fully saturated,we terminated the loop
					if(FragColor.a > 0.99)
						break;

				}


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
				PrintLog("Error in Raycating FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);

				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		PrintLog("Success  in Raycating FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_RayCasting = glCreateProgram();


	glAttachShader(shaderProgramObject_RayCasting, vertexShaderObject);
	glAttachShader(shaderProgramObject_RayCasting, fragmentShaderObject);

	// MOVED BELOW
	//glBindAttribLocation(shaderProgramObject_RayCasting, ATTRIBUTE_POSITION, "aPosition");

	Log = NULL;

	glLinkProgram(shaderProgramObject_RayCasting);

	glGetProgramiv(shaderProgramObject_RayCasting, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_RayCasting, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);

			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_RayCasting, infoLogLength, &written, Log);
				PrintLog("Error in RayCasting shaderObject Linking\nLinking Log : % s\n", Log);

				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		PrintLog("RayCasting shaderObject Linking Successful \n");
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

	// Grid or Axes Rendering 
	//Render_Volume_Box_Axes(modelViewProjectionMatrix);


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

	//Uninitialize_ShaderProgramObject(shaderProgramObject_RayCasting);

}



