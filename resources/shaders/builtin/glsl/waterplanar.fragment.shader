uniform vec4 WATERCOLOR;
uniform sampler2D REFLECTED_TEXTURE;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODELVIEWPROJECTION_MATRIX;
uniform mat4 MODEL_MATRIX;

varying vec2 uv0;
varying vec4 position;
varying vec4 oPos;
varying vec4 normal;
        
    
void main()
{	
	vec4 pos = oPos;
	oPos = MODELVIEWPROJECTION_MATRIX * oPos;
	oPos.x = oPos.x / oPos.w;
	oPos.y = oPos.y / oPos.w;
	oPos.x = (oPos.x + 1.0) / 2.0;
	oPos.y = (oPos.y + 1.0) / 2.0;
    vec4 reflectedColor = texture(REFLECTED_TEXTURE, oPos.xy);
    gl_FragColor = reflectedColor * WATERCOLOR;
}
