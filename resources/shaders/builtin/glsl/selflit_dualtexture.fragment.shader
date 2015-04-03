#version 130

uniform vec4 SELFCOLOR;
uniform sampler2D TEXTUREA;
uniform sampler2D TEXTUREB;

in vec2 uv0;
in vec2 uv0Offset;
in vec2 uv1;
in vec2 uv1Offset;

out vec4 out_color;

void main()
{	
	vec4 textureColorA = texture2D(TEXTUREA, uv0.st + uv0Offset); 
	vec4 textureColorB = texture2D(TEXTUREB, uv1.st + uv1Offset); 
    out_color = textureColorA * textureColorA * SELFCOLOR;
}
