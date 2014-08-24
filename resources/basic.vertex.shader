 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
attribute vec4 POSITION;
attribute vec3 COLOR;
 
varying vec3 vColor;
 
void main()
{
    vColor = COLOR;
    gl_Position = projMatrix * viewMatrix * POSITION ;
}
