#version 130

uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;

uniform sampler2D REFLECTED_TEXTURE;
uniform sampler2D WATER_HEIGHT_MAP;
uniform sampler2D WATER_NORMAL_MAP;
uniform sampler2D SCREEN_BUFFER_TEXTURE;
uniform vec4 LIGHT_POSITION;
uniform vec4 CAMERA_POSITION;
uniform float REFLECTED_COLOR_FACTOR;
uniform float REFRACTED_COLOR_FACTOR;

in vec4 position;
in vec4 oPos;
in vec4 modLocalPos;
in vec4 normal;

out vec4 out_color;
        
void main()
{	
	vec2 texCoords = vec2(oPos.x * 0.5 + 0.5, 0.5 - oPos.z * 0.5);
	vec3 LocalNormal = normalize(texture(WATER_NORMAL_MAP, texCoords.st).rgb);
	vec3 Direction = normalize(position.xyz - CAMERA_POSITION.xyz);
	vec3 Normal = vec3(MODEL_MATRIX * vec4(LocalNormal, 0));
	float h = texture(WATER_HEIGHT_MAP, texCoords.st).g;

 	vec4 reflectOffset = vec4(LocalNormal.x, 0 , LocalNormal.z, 0) * .45 * (h / .002);
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
	//reflectedColor = vec4( normalize(texture(WATER_HEIGHT_MAP, texCoords).rgb) ,1);
	
	vec4 currentColor = texture(SCREEN_BUFFER_TEXTURE, vec2(refractedXCoord, refractedYCoord));
	//currentColor = vec4( texture(WATER_HEIGHT_MAP, texCoords).rgb ,1);
	
	out_color = ((currentColor * REFRACTED_COLOR_FACTOR) + (reflectedColor * REFLECTED_COLOR_FACTOR));
}
