#ifndef _UNIFORMS_H_
#define _UNIFORMS_H_

enum class UniformMaskComponent
{
    ModelViewMatrix=1,
    ModelViewProjectionMatrix=2,
    ProjectionMatrix=4,
    Light=8
};

enum class Uniform
{
	ModelViewMatrix=0,
	ModelViewProjectionMatrix=1,
	ProjectionMatrix=2,
	Light=3,
    _Last=4 // always keep as last entry
};

typedef unsigned int UniformSet;

class Uniforms
{
	static const char* const uniformNames[];

	public:

	static const char * GetUniformName(Uniform uniform);
	static Uniform UniformMaskComponentToUniform(UniformMaskComponent component);
	static UniformMaskComponent UniformToUniformMaskComponent(Uniform uniform);

	static UniformSet CreateUniformSet();
	static void AddUniform(UniformSet * set, Uniform uniform);
	static void RemoveUniform(UniformSet * set, Uniform uniform);

	static bool HasUniform(UniformSet set, Uniform uniform);
};

#endif
