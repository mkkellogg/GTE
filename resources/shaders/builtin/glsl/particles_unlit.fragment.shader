#include "particles_fragment_header.inc"

void main()
{
	vec4 textureColor = texture(PARTICLE_TEXTURE, vUV);
	out_color = vColor *textureColor;
}

