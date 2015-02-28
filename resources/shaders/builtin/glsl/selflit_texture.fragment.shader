uniform vec4 SELFCOLOR;
uniform sampler2D TEXTUREA;
uniform sampler2D TEXTUREB;

varying vec2 uv0;
varying vec2 uv0Offset;

void main()
{	
	vec4 textureColor = texture2D(TEXTUREA, uv0.st + uv0Offset); 
    gl_FragColor = textureColor * SELFCOLOR;
}
