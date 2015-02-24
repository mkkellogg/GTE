precision high float;

vec4 EncodeFloatRGBA( float v ) {
  vec4 enc = vec4(1.0, 255.0, 65025.0, 160581375.0) * v;
  enc = frac(enc);
  enc -= enc.yzww * float4(1.0/255.0,1.0/255.0,1.0/255.0,0.0);
  return enc;
}

void main()
{	
    gl_FragColor = EncodeFloatRGBA(gl_FragCoord.z);
}


