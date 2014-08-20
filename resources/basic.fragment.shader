 
varying vec3 vColor;
vec4 outputF;
 
void main()
{
    outputF = vec4(vColor,1.0);
    gl_FragColor = outputF;
}
