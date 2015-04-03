#version 130

uniform vec4 SELFCOLOR;
uniform sampler2D TEXTUREA;
uniform sampler2D TEXTUREB;

in vec2 uv0;
in vec2 uv0Offset;

out vec4 out_color;

void main()
{	
	vec4 textureColor = texture(TEXTUREA, uv0.st + uv0Offset); 
    out_color = textureColor * SELFCOLOR;
}
