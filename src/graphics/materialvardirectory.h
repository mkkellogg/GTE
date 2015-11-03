#ifndef _GTE_MATERIAL_VAR_DIRECTORY_H_
#define _GTE_MATERIAL_VAR_DIRECTORY_H_

#include "object/enginetypes.h"
#include "base/intmask.h"
#include "stduniforms.h"
#include "stdattributes.h"
#include <unordered_map>
#include <string>

namespace GTE
{
	typedef UInt32 UniformID;
	typedef UInt32 AttributeID;

	template <typename TID, typename TSV, typename TSVU> class MaterialVarDirectory
	{
		public:

		static const TID VarID_Invalid = (TID)-1;

		private:

		static UInt32 nextID;
		static std::unordered_map <std::string, TID> registeredVars;
		static std::unordered_map <TID, std::string> varNames;
		static TID registeredStandardVars[];

		public:

		static TID MaterialVarDirectory::RegisterVarID(std::string name)
		{
			TID foundID = VarID_Invalid;

			auto result = registeredVars.find(name);
			if(result == registeredVars.end())
			{
				foundID = nextID;
				registeredVars[name] = foundID;
				varNames[foundID] = name;
				nextID++;
			}
			else
			{
				foundID = (*result).second;
			}

			TSV stdVar = TSVU::ForName(name);
			if(stdVar != TSV::_None)
			{
				registeredStandardVars[(UInt16)stdVar] = foundID;
			}

			return foundID;
		}

		static TID MaterialVarDirectory::GetVarID(TSV var)
		{
			return registeredStandardVars[(UInt16)var];
		}

		static const std::string* MaterialVarDirectory::GetVarName(TID id)
		{
			auto result = varNames.find(id);
			if(result == varNames.end())
			{
				return nullptr;
			}

			return &(*result).second;
		}
	};

	typedef MaterialVarDirectory<UniformID, StandardUniform, StandardUniforms> UniformDirectory;
	typedef MaterialVarDirectory<AttributeID, StandardAttribute, StandardAttributes> AttributeDirectory;

	template <typename TID, typename TSV, typename TSVU> UInt32 MaterialVarDirectory<TID, TSV, TSVU>::nextID = 0;
	template <typename TID, typename TSV, typename TSVU> std::unordered_map <std::string, TID> MaterialVarDirectory<TID, TSV, TSVU>::registeredVars;
	template <typename TID, typename TSV, typename TSVU> std::unordered_map <TID, std::string> MaterialVarDirectory<TID, TSV, TSVU>::varNames;
	template <typename TID, typename TSV, typename TSVU> TID MaterialVarDirectory<TID, TSV, TSVU>::registeredStandardVars[(UInt16)TSV::_Last];
}

#endif