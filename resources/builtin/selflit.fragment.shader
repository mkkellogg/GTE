 
varying vec4 vColor;
vec4 outputF;
varying vec4 vPosition;

void main()
{	
    outputF = vColor;
    gl_FragColor = outputF;
}
