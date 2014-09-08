#ifndef _SHADER_CATALOG_H_
#define _SHADER_CATALOG_H_

//forward declarations
class EngineObjectManager;

#include "graphics/graphics.h"
#include "shader.h"
//#include "object/engineobjectmanager.h"
#include <vector>
#include <map>

class ShaderCatalog
{
	friend EngineObjectManager;

	protected :

	std::map<int, Shader *> shaders;

	ShaderCatalog();
	~ShaderCatalog();

	public:

	bool AddShader(int id, Shader * shader);
	Shader * GetShader(int id) ;
};

#endif
