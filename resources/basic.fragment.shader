 
varying vec3 vColor;
varying vec2 vUV1;

vec4 outputF;
uniform sampler2D Texture;

vec4 texColor;

void main()
{
	texColor = texture2D(Texture,vUV1);
    outputF = vec4(vColor,1.0) * texColor;
    gl_FragColor = outputF;
}
