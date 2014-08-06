 
varying vec3 Color;
vec4 outputF;
 
void main()
{
    outputF = vec4(Color,1.0);
    gl_FragColor = outputF;
}
