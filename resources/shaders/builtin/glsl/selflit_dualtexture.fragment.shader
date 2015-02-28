uniform vec4 SELFCOLOR;
uniform sampler2D TEXTUREA;
uniform sampler2D TEXTUREB;

varying vec2 uv0;
varying vec2 uv0Offset;
varying vec2 uv1;
varying vec2 uv1Offset;

void main()
{	
	vec4 textureColorA = texture2D(TEXTUREA, uv0.st + uv0Offset); 
	vec4 textureColorB = texture2D(TEXTUREB, uv1.st + uv1Offset); 
    gl_FragColor = textureColorA * textureColorA * SELFCOLOR;
}
