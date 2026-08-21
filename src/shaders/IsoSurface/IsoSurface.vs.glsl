

			
#version 460 core
			
layout (location = 0) in vec3 aPosition;
uniform mat4 u_MVPMatrix;
smooth out vec3 oTexCoords;

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

