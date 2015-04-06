#version 130

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float PIXEL_DISTANCE;
uniform float TIME_FACTOR;

in vec2 vUVTexture0;

out vec4 out_color;
invariant out_color;

float getForce(vec2 coords, vec2 sourceForce)
{
	float force = texture(WATER_HEIGHT_MAP, coords.st).g - sourceForce.g;
	return force;
}

void main()
{
	vec4 full = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st);
    vec2 vh = full.rg;

    float force = 0.0;

    force += 0.707107 * getForce(vUVTexture0.st - vec2(PIXEL_DISTANCE, PIXEL_DISTANCE), vh);
    force += getForce(vUVTexture0.st - vec2(0.0, PIXEL_DISTANCE), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(0.0, PIXEL_DISTANCE)).g - vh.g;
    force += 0.707107 * getForce( vUVTexture0.st + vec2(PIXEL_DISTANCE, -PIXEL_DISTANCE), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(PIXEL_DISTANCE, -PIXEL_DISTANCE)).g - vh.g);

    force += getForce(vUVTexture0.st - vec2(PIXEL_DISTANCE, 0.0), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(PIXEL_DISTANCE, 0.0)).g - vh.g;
    force += getForce(vUVTexture0.st + vec2(PIXEL_DISTANCE, 0.0), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(PIXEL_DISTANCE, 0.0)).g - vh.g;

    force += 0.707107 * getForce(vUVTexture0.st + vec2(-PIXEL_DISTANCE, PIXEL_DISTANCE), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(-PIXEL_DISTANCE, PIXEL_DISTANCE)).g - vh.g);
    force += getForce(vUVTexture0.st + vec2(0.0, PIXEL_DISTANCE), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, PIXEL_DISTANCE)).g - vh.g;
    force += 0.707107 * getForce(vUVTexture0.st + vec2(PIXEL_DISTANCE, PIXEL_DISTANCE), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(PIXEL_DISTANCE, PIXEL_DISTANCE)).g - vh.g);

    force *= .065;

	float oldForce = vh.g; 
    vh.r += force;
    vh.g += vh.r;
    vh.g *= .99;
  //  vh.r *= .99;
    
    vec4 result = vec4(vh, 0, 0.0);

    out_color = result;
}



