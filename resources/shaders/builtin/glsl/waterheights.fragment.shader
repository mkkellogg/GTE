#version 150

precision highp float;

uniform sampler2D WATER_HEIGHT_MAP;
uniform float PIXEL_DISTANCE;
uniform float TIME_FACTOR;

in vec2 vUVTexture0;

out vec4 out_color;

invariant out_color;
invariant vUVTexture0;

void main()
{
      vec4 data = texture2D(WATER_HEIGHT_MAP,  vUVTexture0.st);
      
      /* calculate average neighbor height */
      vec2 dx = vec2(PIXEL_DISTANCE, 0.0);
      vec2 dy = vec2(0.0, PIXEL_DISTANCE);
      float average = (
        texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - dx).r +
        texture2D(WATER_HEIGHT_MAP, vUVTexture0.st - dy).r +
        texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + dx).r +
        texture2D(WATER_HEIGHT_MAP, vUVTexture0.st + dy).r
      ) * 0.25;
      
      /* change the velocity to move toward the average */
      data.g += (average - data.r) * 2.0;
      
      /* attenuate the velocity a little so waves do not last forever */
      data.g *= 0.965;
      
      /* move the vertex along the velocity */
      data.r += data.g;
    
	  out_color = data;
}



