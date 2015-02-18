attribute vec4 POSITION;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
varying vec4 TexCoord0;

void main()
{
    vec4 WVP_Pos = MODELVIEWPROJECTION_MATRIX * vec4(POSITION.xyz, 1.0);
    gl_Position = WVP_Pos.xyww;
    TexCoord0 = POSITION;
}