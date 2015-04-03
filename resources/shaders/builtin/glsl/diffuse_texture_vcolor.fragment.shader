#version 130

uniform sampler2D TEXTURE0;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;

vec4 outputF;
vec4 texColor;

in vec3 vColor;
in vec2 vUVTexture0;
in vec3 vNormal;
in vec4 vPosition;
in vec3 vLightDir;

out vec4 out_color;

void main()
{
	texColor = texture2D(TEXTURE0, vUVTexture0);
	float DiffuseTerm = 0.0;
	
	if(LIGHT_TYPE == 4)
	{
		DiffuseTerm = LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 1)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 normalized_vertex_to_light_vector = -vLightDir;
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 2)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
		float light_dist = length(vertex_to_light_vector);
		float attenForLength = LIGHT_ATTENUATION * light_dist;
		float attenuationFactor = max(1.0-attenForLength,0.0);
		vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
	}
	
	outputF = vec4(vColor,1.0) * texColor * DiffuseTerm * LIGHT_COLOR;
	out_color = outputF;
}
