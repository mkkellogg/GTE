#version 150

uniform sampler2D TEXTURE0;
uniform sampler2D NORMALMAP;
uniform vec4 EYE_POSITION;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;
uniform float SPECULAR_FACTOR;

vec4 texColor;
vec4 outputF;

in vec3 vColor;
in vec2 vUVTexture0;
in vec3 vNormal;
in vec3 vFaceNormal;
in vec3 vTangent;
in vec4 vPosition;

out vec4 out_color;

vec3 calcMappedNormal()
{
    vec3 Normal = normalize(vNormal);
    vec3 Tangent = normalize(vTangent);
    Tangent = normalize(Tangent - dot(Tangent, Normal) * Normal);
    vec3 Bitangent = cross(Tangent, Normal);
    vec3 BumpMapNormal = texture(NORMALMAP, vUVTexture0).xyz;
    BumpMapNormal = 2.0 * BumpMapNormal - vec3(1.0, 1.0, 1.0);
    vec3 NewNormal;
    mat3 TBN = mat3(Tangent, Bitangent, Normal);

    NewNormal = TBN * BumpMapNormal;
    NewNormal = normalize(NewNormal);
    return NewNormal;
}

vec4 calcSpecular(float attenuationFactor, vec3 lightVector, vec3 mappedNormal, vec3 surfaceNormal, vec4 eyePosition, vec4 position, float lightIntensity, vec4 lightColor)
{
	vec4 specularColor = vec4(0,0,0,0);
	vec3 VertexToEye = normalize(vec3(eyePosition - position));
    vec3 LightReflect = normalize(reflect(lightVector, mappedNormal));
	float specularTerm = dot(VertexToEye, LightReflect);
    specularTerm = pow(max(0.0, specularTerm), 32) * attenuationFactor * (lightIntensity) * SPECULAR_FACTOR;
	if(dot(-lightVector,surfaceNormal) <0.0) specularTerm=0.0;
    if (specularTerm > 0) {
        specularColor = lightColor * specularTerm;
 	}
 	return specularColor;
 }

void main()
{
	texColor = texture(TEXTURE0, vUVTexture0);
	float diffuseTerm = 0.0;
	float specularTerm = 0.0;
	vec4 specularColor = vec4(0,0,0,0);
	vec4 diffuseColor = vec4(0,0,0,0);
	vec3 surfaceNormal = normalize(vNormal);
	
	vec3 toLight;
	vec3 lightVector;
	
	if(LIGHT_TYPE == 4)
	{
		diffuseTerm = LIGHT_INTENSITY;
		diffuseColor = LIGHT_COLOR * diffuseTerm;
	}
	else if(LIGHT_TYPE == 1)
	{
		vec3 mappedNormal = calcMappedNormal();
		lightVector = normalize(vec3(LIGHT_DIRECTION));
		toLight = -lightVector;
		
		diffuseTerm = clamp(dot(mappedNormal, toLight), 0.0, 1.0) * LIGHT_INTENSITY;
		//if(dot(-lightVector, surfaceNormal) < 0.0) diffuseTerm=0.0;
		diffuseColor = LIGHT_COLOR * diffuseTerm;
        
        specularColor = calcSpecular(1.0, lightVector, mappedNormal, surfaceNormal, EYE_POSITION, vPosition, LIGHT_INTENSITY, LIGHT_COLOR);
    }
    else if(LIGHT_TYPE == 2)
    {
   		vec3 mappedNormal = calcMappedNormal();
   		lightVector = vec3(vPosition - LIGHT_POSITION);
		toLight = -lightVector;
		lightVector = normalize(lightVector);

		float light_dist = length(toLight);
		float attenForLength = LIGHT_ATTENUATION * light_dist;
		float attenuationFactor = max(1.0-attenForLength,0.0);
	    vec3 normalizedVertexToLight = normalize(toLight);
	    
	    diffuseTerm = clamp(dot(mappedNormal, normalizedVertexToLight), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
		//if(dot(-lightVector, mappedNormal) < 0.0) diffuseTerm=0.0;
	    diffuseColor = LIGHT_COLOR * diffuseTerm;
	    
        specularColor = calcSpecular(attenuationFactor, lightVector, mappedNormal, surfaceNormal, EYE_POSITION, vPosition, LIGHT_INTENSITY, LIGHT_COLOR);
    }
	
    outputF = texColor * (diffuseColor  + specularColor);
    out_color = outputF;
}
