#version 150

uniform sampler2D TEXTUREA;
uniform sampler2D TEXTUREB;

in vec2 uv0;
in vec2 uv0Offset;
in vec2 uv1;
in vec2 uv1Offset;

out vec4 out_color;

void main()
{	
	vec4 textureColorA = texture(TEXTUREA, uv0.st + uv0Offset); 
	vec4 textureColorB = texture(TEXTUREB, uv1.st + uv1Offset); 
	vec4 mA = textureColorA * .4;
	vec4 mB = textureColorB * .6;
    out_color = mA + mB;
}
