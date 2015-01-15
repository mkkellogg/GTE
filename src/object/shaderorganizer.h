/*
 * class: ShaderManager
 *
 * author: Mark Kellogg
 *
 * Class for managing built-in shaders
 *
 */

#ifndef _GTE_SHADER_MANAGER_H_
#define _GTE_SHADER_MANAGER_H_

//forward declarations
class Shader;

#include "object/enginetypes.h"
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

class ShaderOrganizer
{
	std::map<LongMask, ShaderRef> loadedShaders;

	protected:

	public:

	ShaderOrganizer();
	~ShaderOrganizer();

	void AddShader(LongMask properties, ShaderRef shader);
	ShaderRef GetShader(LongMask flags);
};

#endif


