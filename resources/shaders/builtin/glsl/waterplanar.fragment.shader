#version 150

uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;

uniform sampler2D REFLECTED_TEXTURE;
uniform sampler2D WATER_HEIGHT_MAP;
uniform sampler2D SCREEN_BUFFER_TEXTURE;
//uniform vec4 LIGHT_POSITION;
//uniform vec4 CAMERA_POSITION;
uniform float REFLECTED_COLOR_FACTOR;
uniform float REFRACTED_COLOR_FACTOR;

in vec4 oPos;
in vec4 position;
out vec4 out_color;
        
void main()
{	
	vec2 texCoords = vec2(oPos.x * 0.5 + 0.5, 0.5 - oPos.z * 0.5);
	vec4 info = texture(WATER_HEIGHT_MAP, texCoords.st);
	float h = texture(WATER_HEIGHT_MAP, texCoords.st).r;

	vec3 normal =  vec3(info.b, sqrt(1.0 - dot(info.ba, info.ba)), info.a);

	vec4 reflectOffset = vec4(normal.x, 0,  normal.z ,0) * h;
   	reflectOffset = oPos + reflectOffset;
	vec4 projectedReflectPos = MODELVIEWPROJECTION_MATRIX *  reflectOffset;
	projectedReflectPos.x /= projectedReflectPos.w;
	projectedReflectPos.y /= projectedReflectPos.w;
	projectedReflectPos.z /= projectedReflectPos.w;
	projectedReflectPos.x = (projectedReflectPos.x + 1.0) / 2.0;
	projectedReflectPos.y = (projectedReflectPos.y + 1.0) / 2.0;
	
	float reflectedXCoord = projectedReflectPos.x;
	float reflectedYCoord = projectedReflectPos.y;
	float refractedXCoord = projectedReflectPos.x;
	float refractedYCoord = projectedReflectPos.y;
	
	vec4 reflectedColor = texture(REFLECTED_TEXTURE, vec2(reflectedXCoord,reflectedYCoord));
	vec4 currentColor = texture(SCREEN_BUFFER_TEXTURE, vec2(refractedXCoord, refractedYCoord));
	
	out_color = ((currentColor * REFRACTED_COLOR_FACTOR) + (reflectedColor * REFLECTED_COLOR_FACTOR));
}
