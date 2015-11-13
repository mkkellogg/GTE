#include "particles_vertex_header.inc"

void main() 
{
	vColor = COLOR;
	vUV = UVTEXTURE0;
	vec4 quadPos = getQuadPosition();
	gl_Position = PROJECTION_MATRIX * VIEW_MATRIX * quadPos;
}