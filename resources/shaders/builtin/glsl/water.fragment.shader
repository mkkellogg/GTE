uniform vec4 WATERCOLOR;
uniform sampler2D REFLECTED_TEXTURE;
varying vec2 uv0;

void main()
{	
	vec4 reflectedColor = texture2D(REFLECTED_TEXTURE, uv0.st); 
    gl_FragColor = reflectedColor * WATERCOLOR;
}
