#ifndef _GTE_UNIFORM_DIRECTORY_H_
#define _GTE_UNIFORM_DIRECTORY_H_

#include "object/enginetypes.h"
#include "base/intmask.h"
#include "stduniforms.h"
#include <unordered_map>
#include <string>

namespace GTE
{
	typedef UInt32 UniformID;

	class UniformDirectory
	{
		public:

		static const UniformID UniformID_Invalid = (UniformID)-1;

		private:

		static UInt32 nextID;
		static std::unordered_map <std::string, UniformID> registeredUniforms;
		static std::unordered_map <UniformID, std::string> uniformNames;
		static UniformID registeredStandardUniforms[];

		public:

		static UniformID RegisterUniformID(std::string name);
		static UniformID GetUniformID(StandardUniform uniform);
		static const std::string* GetUniformName(UniformID id);
	};
}

#endif
