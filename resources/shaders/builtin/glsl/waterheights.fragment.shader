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
      vec4 data = texture2D(WATER_HEIGHT_MAP,  vUVTexture0.st);
      
      vec2 dx = vec2(PIXEL_DISTANCE, 0.0);
      vec2 dy = vec2(0.0, PIXEL_DISTANCE);
      float average = texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - dx).r + texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - dy).r +
					  texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + dx).r + texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + dy).r;
      average = average / 4;
      
      data.g += (average - data.r) * 2.0;      
      data.g *= 0.975;
      data.r += data.g;
    
	  out_color = data;
}



