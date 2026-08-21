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

	/////////////////////// # VERTEX SHADER # ////////////////////////
	const GLchar* vertexShaderSource = R"(
			
			#version 460 core
			
			layout (location = 0) in vec3 aPosition;
			uniform mat4 u_MVPMatrix;
			smooth out vec3 oTexCoords; //3D texture coordinates for texture lookup in the fragment shader

			void main()
			{
				// step1: // to get the Clipspace position
				gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);


				// step2: //get the 3D texture coordinates from vertex position from step1
				oTexCoords = aPosition + vec3(0.5);
				/* 	step 2 notes:
				we will get the 3D texture coordinates by adding (0.5,0.5,0.5) to the object space vertex position. 
				Since the unit cube is at origin (min: (-0.5,-0.5,-0.5) and max: (0.5,0.5,0.5)), by adding (0.5,0.5,0.5) to the unit cube object space position gives us values from (0,0,0) to (1,1,1)
				*/
		
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
				PrintLog("Error in IsoSurface Vertex Shader .\n VS Compilation Log : %s\n", Log);
				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		PrintLog("Success in IsoSurface  Vertex Shader Compilation \n");
	}



	/////////////////////// # FRAGMENT SHADER # ////////////////////////
	const GLchar* fragmentShaderSource = R"(
			
			#version 460 core

			smooth in vec3 oTexCoords; //3D texture coordinates form vertex shader, interpolated by rasterizer

			uniform sampler3D u_Volume3DSampler; //volume dataset
			uniform vec3 u_cameraPosition; //camera or eye position
			uniform vec3 u_stepSize; //ray step size

			const int MAX_SAMPLES = 300;	//total samples for each ray march step
			const vec3 texMin = vec3(0);	//minimum texture access coordinate
			const vec3 texMax = vec3(1);	//maximum texture access coordinate
			const float DELTA = 0.01;			//the step size for gradient calculation
			const float isoValue = 40/255.0;	//the isovalue for iso-surface detection

			out vec4 FragColor;


			//function to give a more accurate position of where the given iso-value (iso) is found given the initial minimum limit (left) and maximum limit (right)

			vec3 Bisection(vec3 left, vec3 right , float iso)
			{ 
				//loop 4 times
				for(int i=0;i<4;i++)
				{ 
					//get the mid value between the left and right limit
					vec3 midpoint = (right + left) * 0.5;
					//sample the texture at the middle point
					float cM = texture(u_Volume3DSampler, midpoint).x ;
					//check if the value at the middle point is less than the given iso-value
					if(cM < iso)
						//if so change the left limit to the new middle point
						left = midpoint;
					else
						//otherwise change the right limit to the new middle point
						right = midpoint; 
				}
				//finally return the middle point between the left and right limit
				return vec3(right + left) * 0.5;
			}

			//function to calculate the gradient at the given location in the volume dataset
			//The function user center finite difference approximation to estimate the gradient
			vec3 GetGradient(vec3 uvw) 
			{
				vec3 s1, s2;  

				//Using center finite difference 
				s1.x = texture(u_Volume3DSampler, uvw-vec3(DELTA,0.0,0.0)).x ;
				s2.x = texture(u_Volume3DSampler, uvw+vec3(DELTA,0.0,0.0)).x ;

				s1.y = texture(u_Volume3DSampler, uvw-vec3(0.0,DELTA,0.0)).x ;
				s2.y = texture(u_Volume3DSampler, uvw+vec3(0.0,DELTA,0.0)).x ;

				s1.z = texture(u_Volume3DSampler, uvw-vec3(0.0,0.0,DELTA)).x ;
				s2.z = texture(u_Volume3DSampler, uvw+vec3(0.0,0.0,DELTA)).x ;
	 
				return normalize((s1-s2)/2.0); 
			}

			vec4 PhongLighting(vec3 L, vec3 N, vec3 V, float specPower, vec3 diffuseColor)
			{
				float diffuse = max(dot(L,N),0.0);
				vec3 halfVec = normalize(L+V);
				float specular = pow(max(0.00001,dot(halfVec,N)),specPower);	
				return vec4((diffuse*diffuseColor + specular),1.0);
			}

			void main(void)
			{             

				// step 1: save texcoords to local
				vec3 dataPosition = oTexCoords; 

				// step 2: Getting the ray marching direction:
				/*
				get the object space position by subracting 0.5 from the 3D texture coordinates.
				Then subtraact it from camera position and normalize to get the ray marching direction
				*/
				vec3 geomatryDirection = normalize((oTexCoords - vec3(0.5)) - u_cameraPosition);

				// step 3: calculate sub-step size for each Ray marching step
				/*
				multiply the raymarching direction with the step size to get the sub-step size we need to take at each raymarching step
				*/
				vec3 directionStep = geomatryDirection * u_stepSize;

				//flag to indicate if the raymarch loop should terminate
				bool bStop = false;

				for(int i = 0; i<MAX_SAMPLES; i++)
				{
					dataPosition = dataPosition + directionStep; //advance ray by directionStep

					bStop = dot(sign(dataPosition - texMin),sign(texMax - dataPosition)) < 3.0;

					if(bStop)
						break;

					// data fetching from the Red Channel of Volume Texture:
					float fSample = texture(u_Volume3DSampler,dataPosition).r; // sample1
					float fSample2 = texture(u_Volume3DSampler,dataPosition + directionStep).r; // sample2: next sample

					/*
					In case of iso-surface rendering, we do not use compositing. 
					Instead, we find the zero crossing of the volume dataset iso function by sampling two consecutive samples. 
					*/

					if((fSample - isoValue) < 0 && (fSample2 - isoValue) >= 0.0)
					{
						//If there is a zero crossing, we refine the detected iso-surface location by using bisection based refinement.
						vec3 xN = dataPosition;
						vec3 xF = dataPosition + directionStep;
						vec3 tc = Bisection(xN,xF,isoValue);

						//This returns the first hit surface //vFragColor = make_float4(xN,1);
          	
						//To get the shaded iso-surface, we first estimate the normal at the refined position
						vec3 N = GetGradient(tc);					

						//The view vector is simply opposite to the ray marching direction
						vec3 V = -geomatryDirection;

						//We keep the view vector as the light vector to give us a head light
						vec3 L =  V;

						//Finally, we call PhongLighing function to get the final colour with diffuse and specular components. Try changing this call to this
						//vFragColor =  PhongLighting(L,N,V,250,  tc); to get a multi colour
						//iso-surface
						FragColor =  PhongLighting(L,N,V,250, vec3(0.5));	
						break;

					}
				}
				
			}

			//Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
			//Note that since at the time of texture creation, we gave the internal format as GL_RED
			//we can get the sample value from the texture using the red channel. Here, we set all 4
			//components as the sample value in the texture which gives us a shader of grey.
	
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
				PrintLog("Error in IsoSurface FRAGMENT Shader.\nFS Compilation Log : %s\n", Log);

				free(Log);
				Log = NULL;
				uninitialize();
			}
		}
	}
	else
	{
		PrintLog("Success  in IsoSurface FRAGMENT Shader Compilation \n");
	}

	shaderProgramObject_IsoSurface = glCreateProgram();


	glAttachShader(shaderProgramObject_IsoSurface, vertexShaderObject);
	glAttachShader(shaderProgramObject_IsoSurface, fragmentShaderObject);



	Log = NULL;

	glLinkProgram(shaderProgramObject_IsoSurface);

	glGetProgramiv(shaderProgramObject_IsoSurface, GL_LINK_STATUS, &status);

	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject_IsoSurface, GL_INFO_LOG_LENGTH, &infoLogLength);

		if (infoLogLength > 0)
		{
			Log = (char*)malloc(infoLogLength);

			if (Log != NULL)
			{
				GLsizei written;
				glGetProgramInfoLog(shaderProgramObject_IsoSurface, infoLogLength, &written, Log);
				PrintLog("Error in IsoSurface shaderObject Linking\nLinking Log : % s\n", Log);

				free(Log);
				uninitialize();
				return FALSE;
			}
		}
	}
	else
	{
		PrintLog("IsoSurface shaderObject Linking Successful \n");
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



