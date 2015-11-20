#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "layermanager.h"

namespace GTE
{
	LayerManager::LayerManager()
	{
		currentLayerIndex = 0;
	}

	LayerManager::~LayerManager()
	{

	}

	UInt32 LayerManager::AddLayer(const std::string& name)
	{
		if (currentLayerIndex >= MAX_LAYERS)return -1;

		Int32 layerIndex = GetLayerIndex(name);
		if (layerIndex < 0)
		{
			layerIndexes[name] = layerIndex = currentLayerIndex;
			currentLayerIndex++;
		}

		return layerIndex;
	}

	Int32 LayerManager::GetLayerIndex(const std::string& name) const
	{
		if (layerIndexes.find(name) == layerIndexes.end())
		{
			return -1;
		}
		std::map<std::string, UInt32>& mapPtr = const_cast<std::map<std::string, UInt32>&>(layerIndexes);
		return mapPtr[name];
	}

	IntMask LayerManager::GetLayerMask(const std::string& name) const
	{
		Int32 layerIndex = GetLayerIndex(name);
		return GetLayerMask(layerIndex);
	}

	IntMask LayerManager::GetLayerMask(UInt32 layerIndex) const
	{
		IntMask mask = IntMaskUtil::CreateIntMask();
		if (layerIndex < 31)
		{
			IntMaskUtil::SetBit(&mask, layerIndex);
		}
		return mask;
	}

	IntMask LayerManager::RemoveLayerFromMask(IntMask mask, UInt32 layerIndex)
	{
		IntMaskUtil::ClearBit(&mask, layerIndex);
		return mask;
	}

	IntMask LayerManager::MergeLayerMask(IntMask maskA, IntMask maskB)
	{
		return IntMaskUtil::MergeMasks(maskA, maskB);
	}

	IntMask LayerManager::CreateFullLayerMask()
	{
		IntMask mask = IntMaskUtil::CreateIntMask();
		IntMaskUtil::SetAll(&mask);
		return mask;
	}

	IntMask LayerManager::CreateEmptyLayerMask()
	{
		return IntMaskUtil::CreateIntMask();
	}

	Bool LayerManager::ContainsLayer(IntMask mask, UInt32 layerIndex)
	{
		return IntMaskUtil::IsBitSetForMask(mask, layerIndex);
	}

	Bool LayerManager::AtLeastOneLayerInCommon(IntMask a, IntMask b)
	{
		return IntMaskUtil::HaveAtLeastOneInCommon(a, b);
	}
}
