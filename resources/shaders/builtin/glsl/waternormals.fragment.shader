#version 120

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float ODWNMR, WMSDWNMRM2;

varying vec2 vUVTexture0;

invariant gl_FragColor;

void main()
{
    

    /*vec3 y0 = vec3( vec2(ODWNMR, 0.0), texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWNMR, 0.0)).g);
    vec3 y1 = vec3( vec2(0.0, ODWNMR), texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, ODWNMR)).g);
    vec3 y2 = vec3( vec2(-ODWNMR, 0.0), texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(ODWNMR, 0.0)).g);
    vec3 y3 = vec3( vec2(0.0, -ODWNMR), texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(0.0, ODWNMR)).g);*/
    
  /*  float ch = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st).g;
    float h0 = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWNMR, 0.0)).g;
    float h1 = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, ODWNMR)).g;
    float h2 = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(-ODWNMR, 0.0)).g;
    float h3 = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, -ODWNMR)).g;
    
    float heightScale = 10;
    vec3 y0 = vec3( vec2(ODWNMR, 0.0), (ch - h0) * heightScale);
    vec3 y1 = vec3( vec2(0.0, ODWNMR), (ch - h1) * heightScale);
    vec3 y2 = vec3( vec2(-ODWNMR, 0.0), (ch - h2) * heightScale);
    vec3 y3 = vec3( vec2(0.0, -ODWNMR), (ch - h3) * heightScale);
    
    vec3 Normal1 = cross(y0,y1);
    vec3 Normal2 = cross(y1,y2);
    vec3 Normal3 = cross(y2,y3);
    vec3 Normal4 = cross(y3,y0);
    
    vec3 Normal = normalize(Normal1 + Normal4 + Normal3 + Normal2);

    gl_FragColor = vec4(vec3(Normal.x, Normal.z, Normal.y), 1.0);*/
    
    float y[4];

    y[0] = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWNMR, 0.0)).g;
    y[1] = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, ODWNMR)).g;
    y[2] = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(ODWNMR, 0.0)).g;
    y[3] = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(0.0, ODWNMR)).g;

    vec3 Normal = normalize(vec3(y[2] - y[0], WMSDWNMRM2, y[1] - y[3]));

    gl_FragColor = vec4(Normal, 1.0);
}