#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "base/intmask.h"
#include "uniformdirectory.h"
#include "debug/gtedebug.h"

namespace GTE
{
	UInt32 UniformDirectory::nextID = 0;
	std::unordered_map <std::string, UniformID> UniformDirectory::registeredUniforms;
	std::unordered_map <UniformID, std::string> UniformDirectory::uniformNames;
	UniformID UniformDirectory::registeredStandardUniforms[(UInt16)StandardUniform::_Last];

	UniformID UniformDirectory::RegisterUniformID(std::string name)
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

	UniformID UniformDirectory::GetUniformID(StandardUniform uniform)
	{
		return registeredStandardUniforms[(UInt16)uniform];
	}

	const std::string* UniformDirectory::GetUniformName(UniformID id)
	{
		auto result = uniformNames.find(id);
		if(result == uniformNames.end())
		{
			return nullptr;
		}

		return &(*result).second;
	}
}
