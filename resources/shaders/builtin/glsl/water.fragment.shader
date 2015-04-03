uniform vec4 WATERCOLOR;
uniform samplerCube REFLECTED_TEXTURE;
uniform vec4 CAMERA_POSITION;
uniform mat4 MODELVIEW_MATRIX;
uniform mat4 MODEL_MATRIX;
uniform mat4 VIEW_MATRIX;

varying vec4 oPos;
varying vec4 camLocalPos;
varying vec4 position;
varying vec3 normal;

 vec3 CubeMapNormals[] = vec3[](
        vec3(-1.0f, 0.0f, 0.0f),
        vec3(1.0f, 0.0f, 0.0f),
        vec3(0.0f, -1.0f, 0.0f),
        vec3(0.0f, 1.0f, 0.0f),
        vec3(0.0f, 0.0f, -1.0f),
        vec3(0.0f, 0.0f, 1.0f)
    );

 vec3 CubeMapPlanePoints[] = vec3[](
        vec3(-1.0f, 0.5f, 0.5f),
        vec3(1.0f, 0.5f, 0.5f),
        vec3(0.5f, -1.0f, 0.5f),
        vec3(0.5f, 1.0f, 0.5f),
        vec3(0.5f, 0.5f, -1.0f),
        vec3(0.5f, 0.5f, 1.0f)
    );
        
vec3 IntersectCubeMap(vec3 Position, vec3 Direction)
{
    vec3 Point;

    for(int i = 0; i < 6; i++)
    {
        float NdotR = -dot(CubeMapNormals[i], Direction);

        if(NdotR > 0.0)
        {
            float Distance = (dot(CubeMapNormals[i], Position) + 1.0) / NdotR;

            if(Distance > -0.03)
            {
                Point = Direction * Distance + Position;

                if(Point.x > -1.001 && Point.x < 1.001 && Point.y > -1.001 && Point.y < 1.001 && Point.z > -1.001 && Point.z < 1.001)
                {
                    break;
                }
            }
        }
    }

    return vec3(Point.x,Point.yz);
}

vec3 IntersectCubeMap2(vec3 Position, vec3 Direction)
{
    vec3 Point;

    vec3 p = Position;
    vec3 v = normalize(Direction);
            
    for(int i = 0; i < 6; i++)
    {
        float NdotR = dot(CubeMapNormals[i], v);

        if(NdotR > 0.0)
        {
            vec3 q = CubeMapNormals[i];
            vec3 n = CubeMapNormals[i];
            float t = dot(n, (q - p)) / dot(n,v);
            
            Point = p + v * t;
            
             if(Point.x > -1.001 && Point.x < 1.001 && Point.y > -1.001 && Point.y < 1.001 && Point.z > -1.001 && Point.z < 1.001)
                {
                    break;
                }
            //break;
        }
    }

    return vec3(Point.x,Point.yz);
}
    
void main()
{	
	//vec3 camToSurface = position.xyz - CAMERA_POSITION.xyz;
	vec3 camToSurface = oPos.xyz - camLocalPos.xyz;
	//vec3 camToSurface = oPos.xyz - vec3(-camLocalPos.x, camLocalPos.y, -camLocalPos.z);
	
	//vec3 ref = reflect(camToSurface, normalize(normal));
    //vec3 ref = IntersectCubeMap(oPos.xyz,  reflect(camToSurface, normalize(normal)));
	vec3 ref = reflect(camToSurface, normalize(vec3(0,1,0)));
	
	ref = IntersectCubeMap2(oPos.xyz, ref);
	//ref = ref + oPos.xyz;
	ref = vec3(ref.x, -ref.y, -ref.z);
	
	ref = vec3(MODEL_MATRIX * vec4(ref, 0));
		
    vec4 reflectedColor = texture(REFLECTED_TEXTURE, ref);
    gl_FragColor = reflectedColor * WATERCOLOR;
}
