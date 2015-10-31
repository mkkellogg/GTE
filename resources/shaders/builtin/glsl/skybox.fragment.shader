#version 150

uniform samplerCube SKYBOX_TEXTURE;

in vec4 TexCoord0;

out vec4 out_color;
out vec3 vWorldPos;

void main()
{
	out_color = texture(SKYBOX_TEXTURE, TexCoord0.xyz);
}
