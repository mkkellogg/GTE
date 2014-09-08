#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include <fstream>
#include <string.h>
#include <map>

#include "shadercatalog.h"
#include "global/global.h"
#include "ui/debug.h"
#include "graphics/graphics.h"

ShaderCatalog::ShaderCatalog()
{

}

ShaderCatalog::~ShaderCatalog()
{

}

bool ShaderCatalog::AddShader(int id, Shader * shader)
{
	NULL_CHECK(shader, "ShaderCatalog::AddShader -> shader is NULL", false);

	if(!shader->IsLoaded())
	{
		Debug::PrintError("ShaderCatalog::AddShader -> shader is not loaded");
		return false;
	}

	shaders[id] = shader;

	return true;
}

Shader * ShaderCatalog::GetShader(int id)
{
	int count = shaders.count(id);

	if(count > 0)
	{
		return shaders[id];
	}

	Debug::PrintError("ShaderCatalog::GetShader -> shader not found");
	return NULL;
}
