


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

