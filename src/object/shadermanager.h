/*
 * class: ShaderManager
 *
 * author: Mark Kellogg
 *
 * Class for managing built-in shaders
 *
 */

#ifndef _SHADER_MANAGER_H_
#define _SHADER_MANAGER_H_

//forward declarations
class Shader;

#include "assimp/scene.h"
#include <string>
#include "base/longmask.h"
#include <map>

enum class ShaderMaterialCharacteristic
{
	DiffuseColored = 0,
	SpecularColored = 1,
	DiffuseTextured = 2,
	SpecularTextured = 3,
	Bumped = 4,
	EmissiveColored = 5,
	EmissiveTextured = 6,
	VertexColors=7,
	VertexNormals=8
};

class ShaderManager
{
	std::map<LongMask, Shader*> loadedShaders;

	protected:

	public:

	ShaderManager();
	~ShaderManager();

	void AddShader(LongMask properties, Shader * shader);
	Shader * GetShader(LongMask flags);
};

#endif


