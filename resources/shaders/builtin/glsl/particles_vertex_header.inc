attribute vec4 COLOR;
attribute vec2 PARTICLE_SIZE;
attribute float PARTICLE_ROTATION;
attribute float PARTICLE_INDEX;

uniform vec3 VIEW_AXIS_X;
uniform vec3 VIEW_AXIS_Y;
uniform vec3 VIEW_AXIS_Z;

varying vec2 vUV;
varying vec4 vColor;

vec4 getQuadPosition()
{
	vec3 axisX = VIEW_AXIS_X;
	vec3 axisY = VIEW_AXIS_Y;
	vec3 axisZ = VIEW_AXIS_Z;

	axisX *= cos( PARTICLE_ROTATION );
	axisY *= sin( PARTICLE_ROTATION );

	axisX += axisY;
	axisY = cross( axisZ, axisX );

	vec3 edge = vec3( 2.0, PARTICLE_INDEX, 3.0 );
	vec3 test = vec3( PARTICLE_INDEX, 0.5, PARTICLE_INDEX );
	vec3 result = step( edge, test );

	float xFactor = -1.0 + ( result.x * 2.0 );
	float yFactor = -1.0 + ( result.y * 2.0 ) + ( result.z * 2.0 );
	
	axisX *= PARTICLE_SIZE.x * xFactor;
	axisY *= PARTICLE_SIZE.y * yFactor;

	return ( modelMatrix * vec4( position, 1.0 ) ) + vec4( axisX + axisY, 0.0 );
}
