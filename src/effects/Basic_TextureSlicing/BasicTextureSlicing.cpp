#pragma once

#include "BasicTextureSlicing.h"


GLuint shaderProgramObject_Slicer1=0;
GLuint modelViewProjectionUniform_Slicer1=0;
GLuint textureVolumeUniform_Slicer1=0;
GLuint VBO_volume=0;
GLuint VAO_volume=0;


// Shader Type 4: Colomap Classifaction Method : global variables:
glm::vec4 jet_values[9] = {
								glm::vec4(0,0,0.5,0),
								glm::vec4(0,0,1,0.1),
								glm::vec4(0,0.5,1,0.3),
								glm::vec4(0,1,1,0.5),
								glm::vec4(0.5,1,0.5,0.75),
								glm::vec4(1,1,0,0.8),
								glm::vec4(1,0.5,0,0.6),
								glm::vec4(1,0,0,0.5),
								glm::vec4(0.5,0,0,0.0)
};

GLuint shaderProgramObject_Colormap = 0;
GLuint modelViewProjectionUniform_Colormap = 0;
GLuint textureVolumeUniform_Colormap = 0;
GLuint levelOfDetail_Uniform = 0;
GLuint texture_TransferFunction = 0;



// function definitions:


//! Shader Type 1: Basic 3D Texture Slicing Method Definitions:
int Initialize_Slicing_shader(void)
{
	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;

	// code:

	vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\Basic_TextureSlicing\\TextureSlicing.vs.glsl", VERTEX);
	fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\Basic_TextureSlicing\\TextureSlicing.fs.glsl", FRAGMENT);

	shaderProgramObject_Slicer1 = glCreateProgram();

	glAttachShader(shaderProgramObject_Slicer1, vertexShaderObject);
	glAttachShader(shaderProgramObject_Slicer1, fragmentShaderObject);


	if (LinkShaderProgramObject(shaderProgramObject_Slicer1)==FALSE)
	{
		PrintLog("Slicer1  shaderProgramObject_Slicer1 Linking FAILED \n");
	}
	else
	{
		PrintLog("Slicer1  shaderProgramObject_Slicer1 Linking Successful \n");
	}

	glUseProgram(shaderProgramObject_Slicer1);
	glBindAttribLocation(shaderProgramObject_Slicer1, ATTRIBUTE_POSITION, "aPosition");
	modelViewProjectionUniform_Slicer1 = glGetUniformLocation(shaderProgramObject_Slicer1, "u_MVPMatrix");
	textureVolumeUniform_Slicer1 = glGetUniformLocation(shaderProgramObject_Slicer1, "u_Volume3DSampler");
	glUniform1i(textureVolumeUniform_Slicer1, 0);
	glUseProgram(0);


	return (0);
}

void Initialize_Slicing_Geometry(void)
{
	// code:
	glGenVertexArrays(1, &VAO_volume);
	glBindVertexArray(VAO_volume);
	{
		glGenBuffers(1, &VBO_volume);
		glBindBuffer(GL_ARRAY_BUFFER, VBO_volume);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vTextureSlices), 0, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(ATTRIBUTE_POSITION);
		glVertexAttribPointer(ATTRIBUTE_POSITION, 3, GL_FLOAT, GL_FALSE, 0, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	glBindVertexArray(0);

}

void Render_Basic_Volume(void)
{
	// local:

	//setup the camera transform
	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));


	viewDirection = -glm::vec3(ModelViewMatrix[0][2], ModelViewMatrix[1][2], ModelViewMatrix[2][2]);

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;




	// code:


	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaderProgramObject_Slicer1);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_Slicer1, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_Slicer1, 0);
		glBindVertexArray(VAO_volume);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices) / sizeof(vTextureSlices[0]));
		glBindTexture(GL_TEXTURE_3D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);



}

void Update_Basic_Volume(void)
{
	// code:

}

void Uninitialize_Slicing_shader(void)
{
	// code:


	if (VBO_volume)
	{
		glDeleteBuffers(1, &VBO_volume);
		VBO_volume = 0;
	}
	if (VAO_volume)
	{
		glDeleteVertexArrays(1, &VAO_volume);
		VAO_volume = 0;
	}

	if (textureID)
	{
		glDeleteTextures(1, &textureID);
		textureID = 0;
	}


	UninitializeShaders(shaderProgramObject_Slicer1);

}


int FindAbsMax(glm::vec3 v)
{
	// code:

	v = glm::abs(v);
	int max_dim = 0;

	float val = v.x;
	if (v.y > val) {
		val = v.y;
		max_dim = 1;
	}
	if (v.z > val) {
		val = v.z;
		max_dim = 2;
	}

	return max_dim;
}

void Slice_Volume(void)
{
	// local:


	glm::vec3 vertexList[8] = {

		// Minus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),			// 1. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace,fZMinus_BackFace),				// 2. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace,fZMinus_BackFace),				// 3. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace,fZMinus_BackFace),				// 4. Left Top

		// Plus Z-Vertices
		glm::vec3(fXMinus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),			// 5. Left Bottom
		glm::vec3(fXPlus_SideFace,fYMinus_BottomFace, fZPlus_FrontFace),			// 6. Right Bottom
		glm::vec3(fXPlus_SideFace, fYPlus_TopFace, fZPlus_FrontFace),				// 7. Right Top
		glm::vec3(fXMinus_SideFace, fYPlus_TopFace, fZPlus_FrontFace)				// 8. Left Top
	};

	// get the max and min distance of each vertex of the unit cube in the viewing direction
	float maxDistance = glm::dot(viewDirection, vertexList[0]);

	float minDistance = maxDistance;
	int maxIndex = 0;
	int count = 0;

	// code:

	// # 1 : get the distance between the current unit cube vertex and the view vector by dot product

	for (int i = 1; i < 8; i++)
	{
		// 1:
		float fDistance = glm::dot(viewDirection, vertexList[i]);

		// 2:
		if (fDistance > maxDistance)
		{
			maxDistance = fDistance;
			maxIndex = i;
		}

		// 3 : 
		if (fDistance < minDistance)
			minDistance = fDistance;

	}
	// 4 : //find tha abs maximum of the view direction vector
	int max_dim = FindAbsMax(viewDirection);

	minDistance -= EPSILON;
	maxDistance += EPSILON;

	//local variables to store the start, direction vectors, 
	//lambda intersection values
	glm::vec3 vecStart[12];
	glm::vec3 vecDir[12];
	float lambda[12];
	float lambda_inc[12];
	float denom = 0;

	//set the minimum distance as the plane_dist
	//subtract the max and min distances and divide by the 
	//total number of slices to get the plane increment
	float plane_dist = minDistance;
	float plane_dist_inc = (maxDistance - minDistance) / float(num_slices);

	//for all edges
	for (int i = 0; i < 12; i++)
	{
		//get the start position vertex by table lookup
		vecStart[i] = vertexList[edges[edgeList[maxIndex][i]][0]];

		//get the direction by table lookup
		vecDir[i] = vertexList[edges[edgeList[maxIndex][i]][1]] - vecStart[i];

		//do a dot of vecDir with the view direction vector
		denom = glm::dot(vecDir[i], viewDirection);

		//determine the plane intersection parameter (lambda) and 
		//plane intersection parameter increment (lambda_inc)
		if (1.0 + denom != 1.0) {
			lambda_inc[i] = plane_dist_inc / denom;
			lambda[i] = (plane_dist - glm::dot(vecStart[i], viewDirection)) / denom;
		}
		else {
			lambda[i] = -1.0;
			lambda_inc[i] = 0.0;
		}
	}


	// local variables to store the intesected points
	//note that for a plane and sub intersection, we can have 
	//a minimum of 3 and a maximum of 6 vertex polygon
	glm::vec3 intersection[6];

	float dL[12];

	//loop through all slices
	for (int i = num_slices - 1; i >= 0; i--) {

		//determine the lambda value for all edges
		for (int e = 0; e < 12; e++)
		{
			dL[e] = lambda[e] + i * lambda_inc[e];
		}

		//if the values are between 0-1, we have an intersection at the current edge
		//repeat the same for all 12 edges
		if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[0] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[0] = vecStart[1] + dL[1] * vecDir[1];
		}
		else if ((dL[3] >= 0.0) && (dL[3] < 1.0)) {
			intersection[0] = vecStart[3] + dL[3] * vecDir[3];
		}
		else continue;

		if ((dL[2] >= 0.0) && (dL[2] < 1.0)) {
			intersection[1] = vecStart[2] + dL[2] * vecDir[2];
		}
		else if ((dL[0] >= 0.0) && (dL[0] < 1.0)) {
			intersection[1] = vecStart[0] + dL[0] * vecDir[0];
		}
		else if ((dL[1] >= 0.0) && (dL[1] < 1.0)) {
			intersection[1] = vecStart[1] + dL[1] * vecDir[1];
		}
		else {
			intersection[1] = vecStart[3] + dL[3] * vecDir[3];
		}

		if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[2] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[2] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[2] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[6] >= 0.0) && (dL[6] < 1.0)) {
			intersection[3] = vecStart[6] + dL[6] * vecDir[6];
		}
		else if ((dL[4] >= 0.0) && (dL[4] < 1.0)) {
			intersection[3] = vecStart[4] + dL[4] * vecDir[4];
		}
		else if ((dL[5] >= 0.0) && (dL[5] < 1.0)) {
			intersection[3] = vecStart[5] + dL[5] * vecDir[5];
		}
		else {
			intersection[3] = vecStart[7] + dL[7] * vecDir[7];
		}
		if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[4] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[4] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[4] = vecStart[11] + dL[11] * vecDir[11];
		}

		if ((dL[10] >= 0.0) && (dL[10] < 1.0)) {
			intersection[5] = vecStart[10] + dL[10] * vecDir[10];
		}
		else if ((dL[8] >= 0.0) && (dL[8] < 1.0)) {
			intersection[5] = vecStart[8] + dL[8] * vecDir[8];
		}
		else if ((dL[9] >= 0.0) && (dL[9] < 1.0)) {
			intersection[5] = vecStart[9] + dL[9] * vecDir[9];
		}
		else {
			intersection[5] = vecStart[11] + dL[11] * vecDir[11];
		}

		//after all 6 possible intersection vertices are obtained,
		//we calculated the proper polygon indices by using indices of a triangular fan
		int indices[] = { 0,1,2, 0,2,3, 0,3,4, 0,4,5 };

		//! //Using the indices, pass the intersection vertices to the vTextureSlices vector
		for (int i = 0; i < 12; i++)
			vTextureSlices[count++] = intersection[indices[i]];
	}

	//update volume VBO with the new vertices
	glBindBuffer(GL_ARRAY_BUFFER, VBO_volume);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vTextureSlices), &(vTextureSlices[0].x));
	glBindBuffer(GL_ARRAY_BUFFER, 0);


}



//! Shader Type 4: Colormap Classification Method Definitions:

int Initialize_ColormapClassification_shader(void)
{

	// prototype:
	void uninitialize(void);

	// local:

	GLuint vertexShaderObject;
	GLuint fragmentShaderObject;


	// code:

	vertexShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\Colormap\\Colormap.vs.glsl", VERTEX);
	fragmentShaderObject = CreateAndCompileShaderObjects(".\\src\\shaders\\Colormap\\Colormap.fs.glsl", FRAGMENT);

	shaderProgramObject_Colormap = glCreateProgram();

	glAttachShader(shaderProgramObject_Colormap, vertexShaderObject);
	glAttachShader(shaderProgramObject_Colormap, fragmentShaderObject);


	if (LinkShaderProgramObject(shaderProgramObject_Colormap)==FALSE)
	{
		PrintLog("shaderProgramObject_Colormap Linking FAILED \n");
	}
	else
	{
		PrintLog("shaderProgramObject_Colormap Linking Successful \n");
	}


	glUseProgram(shaderProgramObject_Colormap);
	{
		glBindAttribLocation(shaderProgramObject_Colormap, ATTRIBUTE_POSITION, "aPosition");

		modelViewProjectionUniform_Colormap = glGetUniformLocation(shaderProgramObject_Colormap, "u_MVPMatrix");
		textureVolumeUniform_Colormap = glGetUniformLocation(shaderProgramObject_Colormap, "u_Volume3DSampler");
		levelOfDetail_Uniform = glGetUniformLocation(shaderProgramObject_Colormap, "u_LevelOfDetail");

		glUniform1i(textureVolumeUniform_Colormap, 0);
		glUniform1i(levelOfDetail_Uniform, 1);
	}
	glUseProgram(0);


	return (0);
}

void LoadTransferFunction(void)
{
	// local: 
	float pData[256][4];
	int indices[9];


	// code:

	for (int i = 0; i < 9; i++) {
		int index = i * 28;
		pData[index][0] = jet_values[i].x;
		pData[index][1] = jet_values[i].y;
		pData[index][2] = jet_values[i].z;
		pData[index][3] = jet_values[i].w;
		indices[i] = index;
	}


	for (int j = 0; j < 9 - 1; j++)
	{
		float dDataR = (pData[indices[j + 1]][0] - pData[indices[j]][0]);
		float dDataG = (pData[indices[j + 1]][1] - pData[indices[j]][1]);
		float dDataB = (pData[indices[j + 1]][2] - pData[indices[j]][2]);
		float dDataA = (pData[indices[j + 1]][3] - pData[indices[j]][3]);
		int dIndex = indices[j + 1] - indices[j];

		float dDataIncR = dDataR / float(dIndex);
		float dDataIncG = dDataG / float(dIndex);
		float dDataIncB = dDataB / float(dIndex);
		float dDataIncA = dDataA / float(dIndex);
		for (int i = indices[j] + 1; i < indices[j + 1]; i++)
		{
			pData[i][0] = (pData[i - 1][0] + dDataIncR);
			pData[i][1] = (pData[i - 1][1] + dDataIncG);
			pData[i][2] = (pData[i - 1][2] + dDataIncB);
			pData[i][3] = (pData[i - 1][3] + dDataIncA);
		}
	}


	glGenTextures(1, &texture_TransferFunction);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_1D, texture_TransferFunction);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage1D(GL_TEXTURE_1D, 0, GL_RGBA, 256, 0, GL_RGBA, GL_FLOAT, pData);
	//glBindTexture(GL_TEXTURE_1D, 0);


}

void Initialize_ColormapClassification_Geomatry(void)
{
	// local:

	// code:

}

void Render_ColormapClassification_Output(void)
{
	// local:

	glm::mat4 ModelViewMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, dist));

	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, glm::radians(rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
	ModelViewMatrix = glm::rotate(ModelViewMatrix, rotationZ, glm::vec3(0.0f, 0.0f, 1.0f));

	viewDirection = -glm::vec3(ModelViewMatrix[0][2], ModelViewMatrix[1][2], ModelViewMatrix[2][2]);

	glm::mat4 modelViewProjectionMatrix = perspectiveProjMatrix_glm * ModelViewMatrix;

	// code:

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glUseProgram(shaderProgramObject_Colormap);
	{
		glUniformMatrix4fv(modelViewProjectionUniform_Colormap, 1, GL_FALSE, glm::value_ptr(modelViewProjectionMatrix));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, textureID);
		glUniform1i(textureVolumeUniform_Colormap, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_1D, texture_TransferFunction);
		glUniform1i(levelOfDetail_Uniform, 1);

		glBindVertexArray(VAO_volume);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vTextureSlices) / sizeof(vTextureSlices[0]));

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, 0);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_3D, 0);
		glBindVertexArray(0);
	}
	glUseProgram(0);
	glDisable(GL_BLEND);


}

void Update_ColormapClassification_Output(void)
{
	// local:

	// code:

}

void Uninitialize_ColormapClassification_shader(void)
{
	// local:

	// code:

	UninitializeShaders(shaderProgramObject_Colormap);

	if (texture_TransferFunction)
	{
		glDeleteTextures(1, &texture_TransferFunction);
		texture_TransferFunction = 0;
	}


}

