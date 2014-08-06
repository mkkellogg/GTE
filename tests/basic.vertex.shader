 
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
 
attribute vec4 position;
attribute vec3 color;
 
varying vec3 Color;
 
void main()
{
    Color = color;
    gl_Position = projMatrix * viewMatrix * position ;
}
