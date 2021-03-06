void calcPointLightParameters(in vec4 position, in vec4 lightPosition, in float baseAttenuation, out vec3 lightVector, out float attenuation)
{
	lightVector = vec3(position - lightPosition);
	vec3 toLight = -lightVector;
	lightVector = normalize(lightVector);
	attenuation = max(1.0 - baseAttenuation * length(toLight), 0.0);
}

void calcPlanarLightParameters(in vec4 position, in vec3 normal, in vec4 lightPosition, in vec3 lightDirection, in float baseAttenuation, in float lightRange, 
							   in int parallelAttenuationType, in int orthoAttenuationType, out vec3 lightVector, out float attenuation)
{
	lightVector = lightDirection;

	float distParallel = dot(position.xyz, lightVector.xyz) - dot(lightPosition.xyz, lightVector.xyz);
	vec3 toLightPlane = -(distParallel * lightVector.xyz);
	float sgn = distParallel / abs(distParallel);
	attenuation = max(1.0 - baseAttenuation * length(toLightPlane), 0.0) * sgn;

	vec3 positionOnPlane = position.xyz - (dot(position.xyz, lightVector) * lightVector);
	vec3 toLightCenterPlanar = -(positionOnPlane - lightPosition.xyz);
	float lengthToCenterPlanar = length(toLightCenterPlanar);

	if (lengthToCenterPlanar > lightRange)
	{
		attenuation *= max(1.0 - ((lengthToCenterPlanar - lightRange) / 2.0), 0.0);
	}

	vec3 dotTestNormal = normal;
	float dotTestParallel = 1.0;
	if (parallelAttenuationType == 1)
	{
		dotTestParallel = max(dot(-lightVector, dotTestNormal), 0.0);
	}
	else if (parallelAttenuationType == 2)
	{
		float gate = step(0.0, dot(-lightVector, dotTestNormal));
		dotTestParallel = gate + ((1.0 - gate) * (1.0 - dot(lightVector, dotTestNormal)));
	}
	dotTestParallel = max(dotTestParallel, 0.0);

	float dotTestOrtho = 1.0;
	vec3 toLightCenterPlanarNormalized = normalize(toLightCenterPlanar);
	if (orthoAttenuationType == 1)
	{
		dotTestOrtho = max(dot(toLightCenterPlanarNormalized, dotTestNormal), 0.0);
	}
	else if (orthoAttenuationType == 2)
	{
		float gate = step(0.0, dot(toLightCenterPlanarNormalized, dotTestNormal));
		dotTestOrtho = gate + ((1.0 - gate) * (1.0 - dot(-toLightCenterPlanarNormalized, dotTestNormal)));
	}
	dotTestOrtho = max(dotTestOrtho, 0.0);

	attenuation *= dotTestOrtho * dotTestParallel;
}