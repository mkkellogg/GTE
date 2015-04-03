#version 130

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float DROP_RADIUS;
uniform vec2 DROP_POSITION;

in vec2 vUVTexture0;

out vec4 out_color;
invariant out_color;

void main()
{
    vec2 vh = texture(WATER_HEIGHT_MAP, vUVTexture0.st).rg;

    float d = distance(vUVTexture0.st, DROP_POSITION);

    out_color = vec4(vh.r, vh.g - 4.5f * max(DROP_RADIUS - d, 0.0), 0.0, 0.0);
}