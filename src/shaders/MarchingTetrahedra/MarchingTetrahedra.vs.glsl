

#version 460 core
			
layout (location = 0) in vec3 aPosition;
layout (location = 2) in vec3 aNormal;

uniform mat4 u_MVPMatrix;
uniform vec4 u_clippingPlane1;
uniform vec4 u_clippingPlane2;
uniform vec4 u_clippingPlane3;
uniform vec4 u_clippingPlane4;
uniform vec4 u_clippingPlane5;
uniform vec4 u_clippingPlane6;

smooth out vec3 oNormal;

void main()
{
	gl_Position = u_MVPMatrix * vec4(aPosition.xyz, 1.0);
	oNormal = aNormal;

	gl_ClipDistance[0]= dot(u_clippingPlane1.xyz,aPosition)+u_clippingPlane1.w;
	gl_ClipDistance[1]= dot(u_clippingPlane2.xyz,aPosition)+u_clippingPlane2.w;
	gl_ClipDistance[2]= dot(u_clippingPlane3.xyz,aPosition)+u_clippingPlane3.w;
	gl_ClipDistance[3]= dot(u_clippingPlane4.xyz,aPosition)+u_clippingPlane4.w;
	gl_ClipDistance[4]= dot(u_clippingPlane5.xyz,aPosition)+u_clippingPlane5.w;
	gl_ClipDistance[5]= dot(u_clippingPlane6.xyz,aPosition)+u_clippingPlane6.w;
}
	
