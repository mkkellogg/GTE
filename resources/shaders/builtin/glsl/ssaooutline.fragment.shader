#version 130

uniform sampler2D DEPTH_TEXTURE;
uniform mat4 INV_PROJECTION_MATRIX;
uniform float DISTANCE_THRESHHOLD;
uniform vec2 FILTER_RADIUS;
uniform float SCREEN_WIDTH;
uniform float SCREEN_HEIGHT;

in vec3 vNormal;

const int sample_count = 16;
vec2 poisson16[] = vec2[](    // These are the Poisson Disk Samples
                                vec2( -0.94201624,  -0.39906216 ),
                                vec2(  0.94558609,  -0.76890725 ),
                                vec2( -0.094184101, -0.92938870 ),
                                vec2(  0.34495938,   0.29387760 ),
                                vec2( -0.91588581,   0.45771432 ),
                                vec2( -0.81544232,  -0.87912464 ),
                                vec2( -0.38277543,   0.27676845 ),
                                vec2(  0.97484398,   0.75648379 ),
                                vec2(  0.44323325,  -0.97511554 ),
                                vec2(  0.53742981,  -0.47373420 ),
                                vec2( -0.26496911,  -0.41893023 ),
                                vec2(  0.79197514,   0.19090188 ),
                                vec2( -0.24188840,   0.99706507 ),
                                vec2( -0.81409955,   0.91437590 ),
                                vec2(  0.19984126,   0.78641367 ),
                                vec2(  0.14383161,  -0.14100790 )
                            
                               );

out vec4 out_color;

vec4 calculatePosition(vec3 ndcCoords)
{
	vec4 original = vec4(ndcCoords.x, ndcCoords.y, ndcCoords.z, 1);
	original = INV_PROJECTION_MATRIX * original;
	original.x /= original.w;
	original.y /= original.w;
	original.z /= original.w;
	original.w=1;
	return original;
}

void main()
{
    float screenWidth = SCREEN_WIDTH;
	float screenHeight = SCREEN_HEIGHT;
	
	float xOverScreenWidth = (gl_FragCoord.x) / screenWidth;
	float yOverScreenHeight = (gl_FragCoord.y) / screenHeight;

	vec2 texCoords = vec2(xOverScreenWidth, yOverScreenHeight);
	float depth = texture2D(DEPTH_TEXTURE, texCoords).r;
    vec3 ndcCoords = vec3(texCoords.x * 2.0 - 1.0, texCoords.y * 2.0 - 1.0, depth * 2.0 -1.0);
	
	vec3 viewPos = calculatePosition(ndcCoords).xyz;
	vec3 viewNormal = vNormal;
	viewNormal = normalize(viewNormal);

    float ambientOcclusion = 0;
    
	vec2 filterRadius = FILTER_RADIUS;
	float adjustFactor = 1.6 / (pow((depth + 1),8));
	filterRadius *= adjustFactor;

    for (int i = 0; i < sample_count; ++i)
    {
        // sample at an offset specified by the current Poisson-Disk sample and scale it by a radius (has to be in Texture-Space)
        vec2 sampleTexCoord = texCoords + (poisson16[i] * (filterRadius));
        float sampleDepth = texture2D(DEPTH_TEXTURE, sampleTexCoord).r;
        vec3  sampleNdcCoords = vec3(sampleTexCoord.x * 2.0 - 1.0, sampleTexCoord.y * 2.0 - 1.0, sampleDepth  * 2.0 -1.0);
       
        vec3 samplePos = calculatePosition(sampleNdcCoords).xyz;
        vec3 sampleDir = normalize(samplePos - viewPos);

        // angle between SURFACE-NORMAL and SAMPLE-DIRECTION (vector from SURFACE-POSITION to SAMPLE-POSITION)
        float NdotS = max(dot(viewNormal, sampleDir), 0);
        // distance between SURFACE-POSITION and SAMPLE-POSITION
        float VPdistSP = distance(viewPos, samplePos);

        // a = distance function
        float a = 1.0 - smoothstep(DISTANCE_THRESHHOLD, DISTANCE_THRESHHOLD * 2, VPdistSP);
        // b = dot-Product
        float b = NdotS * 1.45;
        
        if(a <0)a=0;
        if(b <0)b = 0;
		float aFactor = (a * b);
		if(aFactor <0)aFactor = 0;
        ambientOcclusion += aFactor;
    }

    float aoValue =  1.0 - (ambientOcclusion / sample_count);
    out_color = vec4(0,0,0,aoValue);  
    // out_color = vec4(aoValue,aoValue,aoValue,1);  
}

