#version 130

out vec4 out_color;

void main()
{	
   	out_color.r = gl_FragCoord.z;
}


