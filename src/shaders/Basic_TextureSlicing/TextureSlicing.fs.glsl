#version 460 core
smooth in vec3 oTexCoords;
uniform sampler3D u_Volume3DSampler; //volume dataset

out vec4 FragColor;		
void main(void)
{             
	FragColor = texture(u_Volume3DSampler, oTexCoords).rrrr;
}

/* Note from CookBook
Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
Note that since at the time of texture creation, we gave the internal format as GL_RED
we can get the sample value from the texture using the red channel. Here, we set all 4
components as the sample value in the texture which gives us a shader of grey.
*/