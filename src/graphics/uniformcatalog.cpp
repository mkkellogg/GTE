#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "uniformcatalog.h"
#include "debug/gtedebug.h"

namespace GTE
{
	UInt32 UniformCatalog::nextID = 0;
	std::unordered_map <std::string, UniformID> UniformCatalog::registeredUniforms;
	std::unordered_map <UniformID, std::string> UniformCatalog::uniformNames;
	UniformID UniformCatalog::registeredStandardUniforms[(UInt16)StandardUniform::_Last];

	UniformID UniformCatalog::RegisterUniformID(std::string name)
	{		
		UniformID foundID = UniformID_Invalid;

		auto result = registeredUniforms.find(name);
		if(result == registeredUniforms.end())
		{
			foundID = nextID;
			registeredUniforms[name] = foundID;
			uniformNames[foundID] = name;
			nextID++;
		}
		else
		{
			foundID = (*result).second;
		}

		StandardUniform stdUniform = StandardUniforms::GetUniformForName(name);
		if(stdUniform != StandardUniform::_None)
		{
			registeredStandardUniforms[(UInt16)stdUniform] = foundID;
		}

		return foundID;
	}

	UniformID UniformCatalog::GetUniformID(StandardUniform uniform)
	{
		return registeredStandardUniforms[(UInt16)uniform];
	}

	const std::string* UniformCatalog::GetUniformName(UniformID id)
	{
		auto result = uniformNames.find(id);
		if(result == uniformNames.end())
		{
			return nullptr;
		}

		return &(*result).second;
	}
}
