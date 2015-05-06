#version 150

uniform sampler2D TEXTURE0;
uniform sampler2D NORMALMAP;
uniform vec4 LIGHT_POSITION;
uniform vec4 LIGHT_DIRECTION;
uniform vec4 LIGHT_COLOR;
uniform float LIGHT_INTENSITY;
uniform float LIGHT_ATTENUATION;
uniform int LIGHT_TYPE;

vec4 texColor;
vec4 outputF;

in vec3 vColor;
in vec2 vUVTexture0;
in vec3 vNormal;
in vec3 vTangent;
in vec4 vPosition;
in vec3 vLightDir;

out vec4 out_color;

vec3 CalcBumpedNormal()
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

void main()
{
	texColor = texture(TEXTURE0, vUVTexture0);
	float DiffuseTerm = 0.0;
	
	if(LIGHT_TYPE == 4)
	{
		DiffuseTerm = LIGHT_INTENSITY;
	}
	else if(LIGHT_TYPE == 1)
	{
		vec3 normalized_normal = CalcBumpedNormal();
		vec3 normalized_vertex_to_light_vector = -vLightDir;
		DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * LIGHT_INTENSITY;
    }
    else if(LIGHT_TYPE == 2)
    {
   		vec3 normalized_normal = CalcBumpedNormal();
		vec3 vertex_to_light_vector = vec3(LIGHT_POSITION - vPosition); 
		float light_dist = length(vertex_to_light_vector);
		float attenForLength = LIGHT_ATTENUATION * light_dist;
		float attenuationFactor = max(1.0-attenForLength,0.0);
	    vec3 normalized_vertex_to_light_vector = normalize(vertex_to_light_vector);
	    DiffuseTerm = clamp(dot(normalized_normal, normalized_vertex_to_light_vector), 0.0, 1.0) * attenuationFactor * LIGHT_INTENSITY;
    }
	
    outputF = vec4(vColor,1.0) * texColor * DiffuseTerm;
    out_color = outputF;
}
