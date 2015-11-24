#ifndef _GTE_MULTIMATERIAL_H_
#define _GTE_MULTIMATERIAL_H_

#include <vector>

#include "engine.h"
#include "object/engineobject.h"

namespace GTE
{
	class MultiMaterial : EngineObject
	{
		// Since this derives from EngineObject, we make this class
		// a friend of EngineObjectManager, and the constructor & destructor
		// protected so its life-cycle can be handled completely by EngineObjectManager.
		friend class EngineObjectManager;

		std::vector<MaterialSharedPtr> materials;

	public:

		MultiMaterial();
		~MultiMaterial();

		UInt32 GetMaterialCount();
		void AddMaterial(MaterialRef material);
		void SetMaterial(UInt32 index, MaterialRef material);
		MaterialRef GetMaterial(UInt32 index);

	};
}

#endif
