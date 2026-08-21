

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

/*
Note from Cookbook:

Here we sample the volume dataset using the 3D texture coordinates from the vertex shader.
Note that since at the time of texture creation, we gave the internal format as GL_RED
we can get the sample value from the texture using the red channel. Here, we set all 4
components as the sample value in the texture which gives us a shader of grey.

*/


