#version 150

precision highp float;

const float PI = 3.141592653589793;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float DROP_RADIUS;
uniform float DROP_STRENGTH;
uniform vec2 DROP_POSITION;

in vec2 vUVTexture0;

out vec4 out_color;

invariant out_color;
invariant vUVTexture0;

void main()
{
	float dropStrength = DROP_STRENGTH;
	float dropRadius = DROP_RADIUS;

    vec4 data = texture(WATER_HEIGHT_MAP, vUVTexture0.st);
     
    float drop = max(0.0, 1.0 - length(DROP_POSITION - vUVTexture0.st) / dropRadius);
    drop = 0.5 - cos(drop * PI) * 0.5;
    data.r += drop * dropStrength;
      
    out_color = data;
}