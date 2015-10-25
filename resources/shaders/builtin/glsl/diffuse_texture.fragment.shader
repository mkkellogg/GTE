#version 150

uniform sampler2D TEXTURE0;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;

vec4 texColor;
vec4 outputF;

in vec3 vNormal;
in vec4 vPosition;
in vec3 vLightDir;
in vec2 vUVTexture0;

out vec4 out_color;

void main()
{
	texColor = texture(TEXTURE0, vUVTexture0);
	
	float DiffuseTerm = 0.0;	
	vec3 normal = normalize(vNormal);
	if(LIGHT_TYPE == 4)
	{
		DiffuseTerm = LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 1)
	{
		DiffuseTerm = clamp(dot(normal, -vLightDir), 0.0, 1.0) * LIGHT_INTENSITY;
 	}
	else if(LIGHT_TYPE == 2)
	{		
		vec3 toLight = vec3(LIGHT_POSITION - vPosition); 
		float attenuationFactor = max(1.0 - LIGHT_ATTENUATION * length(toLight),0.0);
	    vec3 toLightNormalized = normalize(toLight);
    	DiffuseTerm = clamp(dot(normal, toLightNormalized), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 5)
	{
		const int directionalAngleAttenuation = 2;
		const int orthoAngleAttenuation = 2;
		const float lightRange = 55.0;
		vec3 lightDirection = LIGHT_DIRECTION.xyz;
		vec3 lightPosition = LIGHT_POSITION.xyz;
	
		float distParallel = dot(vPosition.xyz, lightDirection.xyz) - dot(lightPosition.xyz, lightDirection.xyz);
		vec3 toLightPlane = -(distParallel * lightDirection.xyz);
		float sgn = distParallel / abs(distParallel);
		float attenuationFactor = max(1.0 - LIGHT_ATTENUATION * length(toLightPlane), 0.0);
		
		vec3 positionOnPlane = vPosition.xyz - (dot(vPosition.xyz, lightDirection) * lightDirection);
	    vec3 toLightCenterPlanar = -(positionOnPlane - lightPosition);
	    float lengthToCenterPlanar = length(toLightCenterPlanar);
	    
	    if(lengthToCenterPlanar > lightRange)
	    {
	    	attenuationFactor *= max(1.0 - ((lengthToCenterPlanar - lightRange) / 2.0), 0.0);
	    }
	    
	    vec3 dotTestNormal = normal;	    
	    float dotTestParallel = 1.0;
	    if (directionalAngleAttenuation == 1)
	    {
	 	    dotTestParallel = max(dot(-lightDirection, dotTestNormal), 0.0);
	    }
	    else if (directionalAngleAttenuation == 2)
	    {
		    float gate = step(0.0,  dot(-lightDirection, dotTestNormal));   
		    dotTestParallel = gate + ((1.0 - gate) * (1.0 - dot(lightDirection, dotTestNormal)));
	    }
	    
		float dotTestOrtho = 1.0;
		vec3 toLightCenterPlanarNormalized = normalize(toLightCenterPlanar);
		if (orthoAngleAttenuation == 1)
	    {
		    dotTestOrtho = max(dot(toLightCenterPlanarNormalized, dotTestNormal), 0.0);	    
	    }
	    else if (orthoAngleAttenuation == 2)
	    {
	     	float gate = step(0.0,  dot(toLightCenterPlanarNormalized, dotTestNormal));   
		    dotTestOrtho = gate + ((1.0 - gate) * (1.0 - dot(-toLightCenterPlanarNormalized, dotTestNormal)));
	    }
	    
		DiffuseTerm = max(max(dotTestParallel, 0.0) * max(dotTestOrtho, 0.0) * attenuationFactor * LIGHT_INTENSITY * sgn, 0.0);	
	}
	
    outputF = texColor * DiffuseTerm * LIGHT_COLOR;
    out_color = outputF;
}
