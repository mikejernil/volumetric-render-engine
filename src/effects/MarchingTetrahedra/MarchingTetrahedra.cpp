#pragma once


#include "MarchingTetrahedra.h"

#include "../../include/Tables.h"


// global variables for Marching Tetrahedra Method:

std::vector<Vertex> vertices;

int XDIM_TM = 0;
int YDIM_TM = 0;
int ZDIM_TM = 0;

GLuint shaderProgramObject_TM = 0;
GLuint modelViewProjectionUniform_TM = 0;
glm::vec3 invDim = glm::vec3(0.0f);

int X_SAMPLING_DIST = 0;
int Y_SAMPLING_DIST = 0;
int Z_SAMPLING_DIST = 0;

//volume data pointer
GLubyte* pVolume=NULL;
GLubyte isoValue;

//vertices vector storing positions and normals
GLuint volumeMarcherVBO = 0;
GLuint volumeMarcherVAO = 0;


// clipping plane members:
GLuint clipFrontFace_uniform = 0;
float fClipPlane_Front = 1.0f;

GLuint clipBackFace_uniform = 0;
float fClipPlane_Back = 0.0f;

GLuint clipRight_uniform = 0;
float fClipPlane_Right = 1.0f;

GLuint clipLeft_uniform = 0;
float fClipPlane_Left = 0.0f;

GLuint clipTop_uniform = 0;
float fClipPlane_Top = 1.0f;

GLuint clipBottom_uniform = 0;
float fClipPlane_Bottom = 0.0f;




// function defintions for Marching Tetrahedra Method


void Update_MarchingTetrahedra_Cube(void)
{
	// code:


	glm::vec3 vertices[8] =
	{
		// Minus Z-Vertices
		glm::vec3(-0.5f,-0.5f,-0.5f), // 1. Left Bottom
		glm::vec3(fXPlus_SideFace,-0.5f,-0.5f),  // 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,-0.5f),  // 3. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace,-0.5f), // 4. Left Top

		// Plus Z-Vertices
		glm::vec3(-0.5f,-0.5f, fZPlus_FrontFace), // 5. Left Bottom
		glm::vec3(fXPlus_SideFace,-0.5f, fZPlus_FrontFace),  // 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),  // 7. Right Top
		glm::vec3(-0.5f, fYPlus_TopFace, fZPlus_FrontFace)  // 8. Left Top
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

	glBindVertexArray(volumeMarcherVAO);
	glBindBuffer(GL_ARRAY_BUFFER, volumeMarcherVBO);
	{
		//pass the obtained vertices from the tetrahedra marcher and pass to the //buffer object memory
		glBufferData(GL_ARRAY_BUFFER, GetTotalVertices_TM() * sizeof(Vertex), GetVertexPointer_TM(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);//enable vertex attribute array for position
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		//enable vertex attribute array for normals
		glEnableVertexAttribArray(ATTRIBUTE_NORMAL);
		glVertexAttribPointer(ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}







void Initialize_TetrahedraMarcher_Constructor()
{
	XDIM_TM = 256;
	YDIM_TM = 256;
	ZDIM_TM = 256;
	pVolume = NULL;
}

void Initialize_TetrahedraMarcher_Shaders()
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


	vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\MarchingTetrahedra\\MarchingTetrahedra.vs.glsl", VERTEX);
	fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\MarchingTetrahedra\\MarchingTetrahedra.fs.glsl", FRAGMENT);


	shaderProgramObject_TM = glCreateProgram();

	glAttachShader(shaderProgramObject_TM, vertexShaderObject);
	glAttachShader(shaderProgramObject_TM, fragmentShaderObject);

	// pre Linking binding code has been MOVED BELOW

	if (LinkShaderProgramObject(shaderProgramObject_TM) == FALSE)
	{
		PrintLog("shaderProgramObject_TM Linking FAILED \n");
	}
	else
	{
		PrintLog("shaderProgramObject_TM Linking Successful \n");
	}


	glUseProgram(shaderProgramObject_TM);
	glBindAttribLocation(shaderProgramObject_TM, ATTRIBUTE_POSITION, "aPosition");
	glBindAttribLocation(shaderProgramObject_TM, ATTRIBUTE_NORMAL, "aNormal");
	modelViewProjectionUniform_TM = glGetUniformLocation(shaderProgramObject_TM, "u_MVPMatrix");

	clipFrontFace_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane1");
	clipBackFace_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane2");

	clipRight_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane3");
	clipLeft_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane4");

	clipTop_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane5");
	clipBottom_uniform = glGetUniformLocation(shaderProgramObject_TM, "u_clippingPlane6");

	glUseProgram(0);

}

void Initialize_TetrahedraMarcher_Geomatry(void)
{
	// code:

	// VAO + VBO CODE for MarchingTetrahedra
	glGenVertexArrays(1, &volumeMarcherVAO);
	glBindVertexArray(volumeMarcherVAO);
	glGenBuffers(1, &volumeMarcherVBO);
	glBindBuffer(GL_ARRAY_BUFFER, volumeMarcherVBO);
	{
		//pass the obtained vertices from the tetrahedra marcher and pass to the //buffer object memory
		glBufferData(GL_ARRAY_BUFFER, GetTotalVertices_TM() * sizeof(Vertex), GetVertexPointer_TM(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);//enable vertex attribute array for position
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);

		//enable vertex attribute array for normals
		glEnableVertexAttribArray(ATTRIBUTE_NORMAL);
		glVertexAttribPointer(ATTRIBUTE_NORMAL, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, normal));

	}
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

}

void SetVolumeDimensions(const int xdim, const int ydim, const int zdim)
{
	XDIM_TM = xdim;
	YDIM_TM = ydim;
	ZDIM_TM = zdim;
	invDim.x = 1.0f / XDIM_TM;
	invDim.y = 1.0f / YDIM_TM;
	invDim.z = 1.0f / ZDIM_TM;
}
void SetNumSamplingVoxels(const int x, const int y, const int z)
{
	X_SAMPLING_DIST = x;
	Y_SAMPLING_DIST = y;
	Z_SAMPLING_DIST = z;
}
void SetIsosurfaceValue(const GLubyte value)
{
	isoValue = value;
}



void SampleVoxel(const int x, const int y, const int z, glm::vec3 scale)
{
	GLubyte cubeCornerValues[8];
	int flagIndex, edgeFlags, i;
	glm::vec3 edgeVertices[12];
	glm::vec3 edgeNormals[12];

	//Make a local copy of the values at the cube's corners
	for (i = 0; i < 8; i++)
	{
		cubeCornerValues[i] = SampleVolume(x + (int)(a2fVertexOffset[i][0] * scale.x),
			y + (int)(a2fVertexOffset[i][1] * scale.y),
			z + (int)(a2fVertexOffset[i][2] * scale.z));
	}

	//Find which vertices are inside of the surface and which are outside
	//Obtain a flagIndex based on if the value at the cube vertex is less 
	//than the given isovalue
	flagIndex = 0;
	for (i = 0; i < 8; i++)
	{
		if (cubeCornerValues[i] <= isoValue)
			flagIndex |= 1 << i;
	}

	//Find which edges are intersected by the surface
	edgeFlags = aiCubeEdgeFlags[flagIndex];

	//If the cube is entirely inside or outside of the surface, then there will be no intersections
	if (edgeFlags == 0)
	{
		return;
	}

	//for all edges
	for (i = 0; i < 12; i++)
	{
		//if there is an intersection on this edge
		if (edgeFlags & (1 << i))
		{
			//get the offset 
			float offset = GetOffset(cubeCornerValues[a2iEdgeConnection[i][0]], cubeCornerValues[a2iEdgeConnection[i][1]]);

			//use offset to get the vertex position
			edgeVertices[i].x = x + (a2fVertexOffset[a2iEdgeConnection[i][0]][0] + offset * a2fEdgeDirection[i][0]) * scale.x;
			edgeVertices[i].y = y + (a2fVertexOffset[a2iEdgeConnection[i][0]][1] + offset * a2fEdgeDirection[i][1]) * scale.y;
			edgeVertices[i].z = z + (a2fVertexOffset[a2iEdgeConnection[i][0]][2] + offset * a2fEdgeDirection[i][2]) * scale.z;

			//use the vertex position to get the normal
			edgeNormals[i] = GetNormal((int)edgeVertices[i].x, (int)edgeVertices[i].y, (int)edgeVertices[i].z);
		}
	}

	//Draw the triangles that were found.  There can be up to five per cube
	for (i = 0; i < 5; i++)
	{
		if (a2iTriangleConnectionTable[flagIndex][3 * i] < 0)
			break;

		for (int j = 0; j < 3; j++)
		{
			int vertex = a2iTriangleConnectionTable[flagIndex][3 * i + j];
			Vertex v;
			v.normal = (edgeNormals[vertex]);
			v.pos = (edgeVertices[vertex]) * invDim;
			vertices.push_back(v);
		}
	}
}

void MarchVolume()
{
	vertices.clear();
	int dx = XDIM_TM / X_SAMPLING_DIST;
	int dy = YDIM_TM / Y_SAMPLING_DIST;
	int dz = ZDIM_TM / Z_SAMPLING_DIST;
	glm::vec3 scale = glm::vec3(dx, dy, dz);
	for (int z = 0; z < ZDIM_TM; z += dz)
	{
		for (int y = 0; y < YDIM_TM; y += dy)
		{
			for (int x = 0; x < XDIM_TM; x += dx)
			{
				SampleVoxel(x, y, z, scale);
			}
		}
	}
}

size_t GetTotalVertices_TM()
{
	return vertices.size();
}

Vertex* GetVertexPointer_TM()
{
	return  &vertices[0];
}

GLubyte SampleVolume(const int x, const int y, const int z) {
	int index = (x + (y * XDIM_TM)) + z * (XDIM_TM * YDIM_TM);
	if (index < 0)
		index = 0;
	if (index >= XDIM_TM * YDIM_TM * ZDIM_TM)
		index = (XDIM_TM * YDIM_TM * ZDIM_TM) - 1;
	return pVolume[index];
}

glm::vec3 GetNormal(const int x, const int y, const int z) {
	glm::vec3 N;
	N.x = (SampleVolume(x - 1, y, z) - SampleVolume(x + 1, y, z)) * 0.5f;
	N.y = (SampleVolume(x, y - 1, z) - SampleVolume(x, y + 1, z)) * 0.5f;
	N.z = (SampleVolume(x, y, z - 1) - SampleVolume(x, y, z + 1)) * 0.5f;
	return glm::normalize(N);
}
float GetOffset(const GLubyte v1, const GLubyte v2) {
	float delta = (float)(v2 - v1);
	if (delta == 0)
		return 0.5f;
	else
		return (isoValue - v1) / delta;
}

void Render_MarchingTetrahedra(void)
{
	// local:

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;
	glm::vec3 cameraPosition = glm::vec3(glm::inverse(ModelViewMatrix) * glm::vec4(0.0, 0.0, 0.0, 1.0));


	glm::vec4 clipPlane_Front = glm::vec4(0.0f, 0.0f, -1.0f, fClipPlane_Front);
	glm::vec4 clipPlane_Back = glm::vec4(0.0f, 0.0f, 1.0f, -fClipPlane_Back);

	glm::vec4 clipPlane_Right = glm::vec4(-1.0f, 0.0f, 0.0f, fClipPlane_Right);
	glm::vec4 clipPlane_Left = glm::vec4(1.0f, 0.0f, 0.0f, -fClipPlane_Left);

	glm::vec4 clipPlane_Top = glm::vec4(0.0f, -1.0f, 0.0f, fClipPlane_Top);
	glm::vec4 clipPlane_Bottom = glm::vec4(0.0f, 1.0f, 0.0f, -fClipPlane_Bottom);

	// code:


	if (bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glm::mat4 TranslationMatrix_2 = glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -0.5f));

	glEnable(GL_CLIP_DISTANCE0);
	glEnable(GL_CLIP_DISTANCE1);
	glEnable(GL_CLIP_DISTANCE2);
	glEnable(GL_CLIP_DISTANCE3);
	glEnable(GL_CLIP_DISTANCE4);
	glEnable(GL_CLIP_DISTANCE5);
	glUseProgram(shaderProgramObject_TM);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_TM, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix * TranslationMatrix_2));//pass the shader uniform

		glUniform4fv(clipFrontFace_uniform, 1, glm::value_ptr(clipPlane_Front));
		glUniform4fv(clipBackFace_uniform, 1, glm::value_ptr(clipPlane_Back));

		glUniform4fv(clipRight_uniform, 1, glm::value_ptr(clipPlane_Right));
		glUniform4fv(clipLeft_uniform, 1, glm::value_ptr(clipPlane_Left));

		glUniform4fv(clipTop_uniform, 1, glm::value_ptr(clipPlane_Top));
		glUniform4fv(clipBottom_uniform, 1, glm::value_ptr(clipPlane_Bottom));

		glBindVertexArray(volumeMarcherVAO);
		glDrawArrays(GL_TRIANGLES, 0, GetTotalVertices_TM());
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glDisable(GL_CLIP_DISTANCE5);
	glDisable(GL_CLIP_DISTANCE4);
	glDisable(GL_CLIP_DISTANCE3);
	glDisable(GL_CLIP_DISTANCE2);
	glDisable(GL_CLIP_DISTANCE1);
	glDisable(GL_CLIP_DISTANCE0);

	//restore the default polygon mode
	if (bWireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

}

void Update_MarchingTetrahedra()
{
	// code:
}

void Uninitialize_MarchingTetrahedra(void)
{
	// code:

	UninitializeShaders(shaderProgramObject_TM);


	if (volumeMarcherVBO)
	{
		glDeleteBuffers(1, &volumeMarcherVBO);
		volumeMarcherVBO = 0;
	}

	if (volumeMarcherVAO)
	{
		glDeleteVertexArrays(1, &volumeMarcherVAO);
		volumeMarcherVAO = 0;
	}

	if (pVolume)
	{
		free(pVolume);
		pVolume = NULL;
	}

	if (!vertices.empty())
	{
		vertices.clear();
	}
}



