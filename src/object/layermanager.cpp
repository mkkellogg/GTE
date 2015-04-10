#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "layermanager.h"

LayerManager::LayerManager()
{
	currentLayerIndex = 0;
}

LayerManager::~LayerManager()
{

}

unsigned int LayerManager::AddLayer(const std::string& name)
{
	if(currentLayerIndex >= MAX_LAYERS)return -1;

	int layerIndex = GetLayerIndex(name);
	if(layerIndex < 0)
	{
		layerIndexes[name] = layerIndex = currentLayerIndex;
		currentLayerIndex++;
	}

	return layerIndex;
}

int LayerManager::GetLayerIndex(const std::string& name) const
{
	if(layerIndexes.find(name) == layerIndexes.end())
	{
		return -1;
	}
	std::map<std::string, unsigned int>& mapPtr = const_cast<std::map<std::string, unsigned int>&>(layerIndexes);
	return mapPtr[name];
}

IntMask LayerManager::GetLayerMask(const std::string& name) const
{
	int layerIndex = GetLayerIndex(name);
	return GetLayerMask(layerIndex);
}

IntMask LayerManager::GetLayerMask(unsigned int layerIndex) const
{
	IntMask mask = IntMaskUtil::CreateIntMask();
	if(layerIndex < 31)
	{
		IntMaskUtil::SetBit(&mask, layerIndex);
	}
	return mask;
}

IntMask LayerManager::RemoveLayerFromMask(IntMask mask, unsigned int layerIndex)
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

bool LayerManager::ContainsLayer(IntMask mask, unsigned int layerIndex)
{
	return IntMaskUtil::IsBitSetForMask(mask, layerIndex);
}

bool LayerManager::AtLeastOneLayerInCommon(IntMask a, IntMask b)
{
	return IntMaskUtil::HaveAtLeastOneInCommon(a,b);
}
