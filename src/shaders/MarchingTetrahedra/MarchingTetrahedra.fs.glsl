

			
#version 460 core

smooth in vec3 oNormal;
out vec4 FragColor;

void main(void)
{             

	FragColor = vec4(oNormal,1.0);
}
	

