#version 130

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float ODWHMR;
uniform float TIME_FACTOR;

in vec2 vUVTexture0;

out vec4 out_color;
invariant out_color;

float getForce(vec2 coords, vec2 sourceForce)
{
	float force = texture2D(WATER_HEIGHT_MAP, coords.st).g - sourceForce.g;
	return force;
}

void main()
{
	vec4 full = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st);
    vec2 vh = full.rg;

    float force = 0.0;

    force += 0.707107 * getForce(vUVTexture0.st - vec2(ODWHMR, ODWHMR), vh);
    force += getForce(vUVTexture0.st - vec2(0.0, ODWHMR), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(0.0, ODWHMR)).g - vh.g;
    force += 0.707107 * getForce( vUVTexture0.st + vec2(ODWHMR, -ODWHMR), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWHMR, -ODWHMR)).g - vh.g);

    force += getForce(vUVTexture0.st - vec2(ODWHMR, 0.0), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - vec2(ODWHMR, 0.0)).g - vh.g;
    force += getForce(vUVTexture0.st + vec2(ODWHMR, 0.0), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWHMR, 0.0)).g - vh.g;

    force += 0.707107 * getForce(vUVTexture0.st + vec2(-ODWHMR, ODWHMR), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(-ODWHMR, ODWHMR)).g - vh.g);
    force += getForce(vUVTexture0.st + vec2(0.0, ODWHMR), vh); //texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(0.0, ODWHMR)).g - vh.g;
    force += 0.707107 * getForce(vUVTexture0.st + vec2(ODWHMR, ODWHMR), vh); //(texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + vec2(ODWHMR, ODWHMR)).g - vh.g);

    force *= .065;

	float oldForce = vh.g; 
    vh.r += force;
    vh.g += vh.r;
    vh.g *= .99;
  //  vh.r *= .99;
    
    vec4 result = vec4(vh, 0, 0.0);

    out_color = result;
}



