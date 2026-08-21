			
#version 460 core
			
layout (location = 0) in vec3 aPosition;
uniform mat4 u_MVPMatrix;
void main()
{
	gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);
}

