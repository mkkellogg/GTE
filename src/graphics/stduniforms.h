#ifndef _UNIFORMS_H_
#define _UNIFORMS_H_

enum class StandardUniformMaskComponent
{
    ModelViewMatrix=1,
    ModelViewProjectionMatrix=2,
    ProjectionMatrix=4,
    LightPosition=8,
    LightDirection=16,
    LightColor=32
};

enum class StandardUniform
{
	ModelViewMatrix=0,
	ModelViewProjectionMatrix=1,
	ProjectionMatrix=2,
	LightPosition=3,
	LightDirection=4,
	LightColor=5,
    _Last=6 // always keep as last entry
};

typedef unsigned int StandardUniformSet;

class StandardUniforms
{
	static const char* const uniformNames[];

	public:

	static const char * GetUniformName(StandardUniform uniform);
	static StandardUniform UniformMaskComponentToUniform(StandardUniformMaskComponent component);
	static StandardUniformMaskComponent UniformToUniformMaskComponent(StandardUniform uniform);

	static StandardUniformSet CreateUniformSet();
	static void AddUniform(StandardUniformSet * set, StandardUniform uniform);
	static void RemoveUniform(StandardUniformSet * set, StandardUniform uniform);

	static bool HasUniform(StandardUniformSet set, StandardUniform uniform);
};

#endif
