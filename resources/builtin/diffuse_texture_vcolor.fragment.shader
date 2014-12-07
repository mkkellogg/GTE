 
varying vec3 vColor;
varying vec2 vUVTexture0;

vec4 outputF;
uniform sampler2D TEXTURE0;

uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_ISDIRECTIONAL;

vec4 texColor;

varying vec3 vNormal;
varying vec4 vPosition;
varying vec3 vLightDir;

void main()
{
	texColor = texture2D(TEXTURE0, vUVTexture0);
	float DiffuseTerm = 1.0;
	
	if(LIGHT_ISDIRECTIONAL == 0)
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
		float light_dist = length(vertex_to_light_vector);
		float attenForLength = LIGHT_ATTENUATION * light_dist;
		float attenuationFactor = max(1.0-attenForLength,0.0);
		vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
	}
	else
	{
		vec3 normalized_normal = normalize(vNormal);
		vec3 normalized_vertex_to_light_vector = -vLightDir;
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * LIGHT_INTENSITY;
	}
	
	outputF = vec4(vColor,1.0) * texColor * DiffuseTerm * vec4(1.0,1.0,1.0,1.0);
	gl_FragColor = outputF;
}
