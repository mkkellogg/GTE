attribute vec4 customColor;
attribute vec2 size;
attribute float rotation;
attribute float customIndex;
varying vec2 vUV;
varying vec4 vColor;
uniform vec3 cameraaxisx;
uniform vec3 cameraaxisy;
uniform vec3 cameraaxisz;

varying vec2 vUV;
varying vec4 vColor;
uniform sampler2D texture;

vec4 getQuadPosition()
{
	vec3 axisX = cameraaxisx;
	vec3 axisY = cameraaxisy;
	vec3 axisZ = cameraaxisz;

	axisX *= cos( rotation );
	axisY *= sin( rotation );

	axisX += axisY;
	axisY = cross( axisZ, axisX );

	vec3 edge = vec3( 2.0, customIndex, 3.0 );
	vec3 test = vec3( customIndex, 0.5, customIndex );
	vec3 result = step( edge, test );

	float xFactor = -1.0 + ( result.x * 2.0 );
	float yFactor = -1.0 + ( result.y * 2.0 ) + ( result.z * 2.0 );
	
	axisX *= size.x * xFactor;
	axisY *= size.y * yFactor;

	return ( modelMatrix * vec4( position, 1.0 ) ) + vec4( axisX + axisY, 0.0 );
}
