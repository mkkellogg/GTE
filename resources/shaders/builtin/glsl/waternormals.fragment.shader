#version 150

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float PIXEL_DISTANCE;
uniform float PIXEL_DISTANCEX2;

in vec2 vUVTexture0;

out vec4 out_color;

invariant out_color;
invariant vUVTexture0;

void main()
{   
    float y[4];

    y[0] = texture(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(PIXEL_DISTANCE, 0.0)).g;
    y[1] = texture(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, PIXEL_DISTANCE)).g;
    y[2] = texture(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(PIXEL_DISTANCE, 0.0)).g;
    y[3] = texture(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(0.0, PIXEL_DISTANCE)).g;

    vec3 Normal = normalize(vec3(y[2] - y[0], PIXEL_DISTANCEX2, y[1] - y[3]));

    out_color = vec4(Normal, 1.0);
}