uniform mat4 MODELVIEWPROJECTION_MATRIX;

attribute vec4 POSITION;
 
void main()
{
    gl_Position = MODELVIEWPROJECTION_MATRIX * POSITION ;
}
