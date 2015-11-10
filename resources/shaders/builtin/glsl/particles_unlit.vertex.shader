#include "particles_vertex_header.inc"

void main() 
{
	vColor = COLOR;
	vUV = uv;
	vec4 quadPos = getQuadPosition();
	gl_Position = projectionMatrix * viewMatrix * quadPos;
}