#version 150

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float PIXEL_DISTANCE;

in vec2 vUVTexture0;

out vec4 out_color;

invariant out_color;
invariant vUVTexture0;

void main()
{  
    vec4 data = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st);
      
    vec3 dx = vec3(PIXEL_DISTANCE, texture2D(WATER_HEIGHT_MAP, vec2(vUVTexture0.s + PIXEL_DISTANCE, vUVTexture0.t)).r - data.r, 0.0);
    vec3 dy = vec3(0.0, texture2D(WATER_HEIGHT_MAP, vec2(vUVTexture0.s, vUVTexture0.t + PIXEL_DISTANCE)).r - data.r, PIXEL_DISTANCE);
    data.ba = normalize(cross(dy, dx)).xz;
      
    out_color = data;
}