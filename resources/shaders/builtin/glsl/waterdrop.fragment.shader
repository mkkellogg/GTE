#version 120

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float DROP_RADIUS;
uniform vec2 DROP_POSITION;

varying vec2 vUVTexture0;

invariant gl_FragColor;

void main()
{
    vec2 vh = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st).rg;

    float d = distance(vUVTexture0.st, DROP_POSITION);

    gl_FragColor = vec4(vh.r, vh.g - 4.5f * max(DROP_RADIUS - d, 0.0), 0.0, 0.0);
}