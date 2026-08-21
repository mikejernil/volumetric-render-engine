
#version 460 core

smooth in vec3 oTexCoords;
uniform sampler3D u_Volume3DSampler;
uniform sampler1D u_LevelOfDetail;

out vec4 FragColor;		
void main(void)
{             

	FragColor = texture(u_LevelOfDetail, texture(u_Volume3DSampler,oTexCoords).r);
}
