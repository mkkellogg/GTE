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
	  float PIXEL_DISTANCE2X = PIXEL_DISTANCE * 2.0;
      vec4 data = texture(WATER_HEIGHT_MAP,  vUVTexture0.st);
      
      vec2 dx = vec2(PIXEL_DISTANCE, 0.0);
      vec2 dy = vec2(0.0, PIXEL_DISTANCE);

	  float average = texture(WATER_HEIGHT_MAP, vUVTexture0.st - dx).r + texture(WATER_HEIGHT_MAP, vUVTexture0.st - dy).r +
					  texture(WATER_HEIGHT_MAP, vUVTexture0.st + dx).r + texture(WATER_HEIGHT_MAP, vUVTexture0.st + dy).r;

	  average /= 4.0;
      
      data.g += (average - data.r) * 2.0;      
      data.g *= 0.987;
      data.r += data.g;
    
	  out_color = data;
}



