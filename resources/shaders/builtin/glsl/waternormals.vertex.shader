#version 130

precision highp float;

in vec4 POSITION;

out vec2 vUVTexture0;

invariant gl_Position;
invariant vUVTexture0;

void main()
{
    vUVTexture0 = POSITION.xy;
    gl_Position = POSITION * 2.0 - 1.0;
}
