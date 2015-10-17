#version 150

uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 PROJECTION_MATRIX;
uniform mat4 VIEW_MATRIX;

uniform sampler2D REFLECTED_TEXTURE;
uniform sampler2D WATER_HEIGHT_MAP;
uniform sampler2D SCREEN_BUFFER_TEXTURE;
//uniform vec4 LIGHT_POSITION;
//uniform vec4 CAMERA_POSITION;
uniform float REFLECTED_COLOR_FACTOR;
uniform float REFRACTED_COLOR_FACTOR;
uniform float PIXEL_DISTANCE;

in vec4 oPos;
in vec4 position;
in vec4 oWorldNormal;
out vec4 out_color;

vec3 getAverageNormal(vec2 coord)
{
	vec4 main = texture(WATER_HEIGHT_MAP, coord.st);
	vec4 yh = texture(WATER_HEIGHT_MAP, vec2(coord.s, coord.t + PIXEL_DISTANCE));
	vec4 yl = texture(WATER_HEIGHT_MAP, vec2(coord.s, coord.t - PIXEL_DISTANCE));
	vec4 xl = texture(WATER_HEIGHT_MAP, vec2(coord.s - PIXEL_DISTANCE, coord.t));
	vec4 xh = texture(WATER_HEIGHT_MAP, vec2(coord.s + PIXEL_DISTANCE, coord.t));

	vec4 total =(main + main + yh + yl + xh + xl) / 6;
	vec3 result = vec3(total.b, sqrt(1.0 - dot(total.ba, total.ba)), total.a);

	return result;
}
        
void main()
{
	vec2 texCoords = vec2(oPos.x * 0.5 + 0.5, 0.5 - oPos.z * 0.5);
	float h = texture(WATER_HEIGHT_MAP, texCoords.st).r;

	vec3 avgNormal = getAverageNormal(texCoords.st);
	vec3 avgNormalWorld = mat3(MODEL_MATRIX) * avgNormal;

	float effectFactor = .2;

	avgNormal = normalize((avgNormal + vec3(0, 1, 0)) / 2.0);

	vec4 reflectOffset = vec4(avgNormal.x, 0, avgNormal.z, 0) * effectFactor;
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
