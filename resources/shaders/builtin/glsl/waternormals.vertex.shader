#version 120

precision highp float;

attribute vec4 POSITION;

varying vec2 vUVTexture0;

invariant gl_Position;
invariant vUVTexture0;

void main()
{
    vUVTexture0 = POSITION.xy;
    gl_Position = POSITION * 2.0 - 1.0;
}
