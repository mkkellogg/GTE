#version 150

precision highp float;

 const float PI = 3.141592653589793;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float DROP_RADIUS;
uniform vec2 DROP_POSITION;

in vec2 vUVTexture0;

out vec4 out_color;

invariant out_color;
invariant vUVTexture0;

void main()
{
	float strength = 2.3;
    vec4 data = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st);
     
    float drop = max(0.0, 1.0 - length(DROP_POSITION - vUVTexture0.st) / DROP_RADIUS);
    drop = 0.5 - cos(drop * PI) * 0.5;
    data.r += drop * strength;
      
    out_color = data;
}