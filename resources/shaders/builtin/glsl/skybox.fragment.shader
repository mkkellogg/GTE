#version 130

uniform samplerCube SKYBOX_TEXTURE;

in vec4 TexCoord0;

out vec4 out_color;

void main()
{
    out_color = textureCube(SKYBOX_TEXTURE, TexCoord0.xyz);
}
