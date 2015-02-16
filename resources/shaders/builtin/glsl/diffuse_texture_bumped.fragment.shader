 
varying vec2 vUVTexture0;

vec4 outputF;
uniform sampler2D TEXTURE0;
uniform sampler2D TEXTURE1;

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;

vec4 texColor;

varying vec3 vNormal;
varying vec4 vPosition;
varying vec3 vLightDir;

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
	
    outputF = texColor * DiffuseTerm * LIGHT_COLOR;
    gl_FragColor = outputF;
}
