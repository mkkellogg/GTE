#version 150

uniform mat4 MODELVIEWPROJECTION_MATRIX;
in vec4 POSITION;

uniform mat4 TEXTURETRANSFORM_MATRIX;

out vec4 TexCoord0;

void main()
{
    vec4 WVP_Pos = MODELVIEWPROJECTION_MATRIX * vec4(POSITION.xyz, 1.0);
    TexCoord0 = TEXTURETRANSFORM_MATRIX * POSITION;    
    gl_Position = WVP_Pos.xyww;
}