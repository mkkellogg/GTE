#include "particles_fragment_header.inc"

void main()
{
	vec4 textureColor = texture2D(texture, vUV);
	gl_FragColor = vColor * textureColor;
}

