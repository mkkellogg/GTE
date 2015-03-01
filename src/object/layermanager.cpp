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
	if(layerIndex >= 0)
	{
		IntMaskUtil::SetBit(&mask, layerIndex);
	}
	return mask;
}
