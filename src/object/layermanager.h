#ifndef _GTE_LAYERMANAGER_H_
#define _GTE_LAYERMANAGER_H_

//forward declarations
class EngineObjectManager;

#include <string>
#include <map>
#include "base/intmask.h"

class LayerManager
{
	friend class EngineObjectManager;

	const unsigned int MAX_LAYERS = 32;
	unsigned int currentLayerIndex;
	std::map<std::string, unsigned int> layerIndexes;

	LayerManager();
	~LayerManager();

    public:

	unsigned int AddLayer(const std::string& name);
	int GetLayerIndex(const std::string& name) const;
	IntMask GetLayerMask(const std::string& name) const;
	IntMask GetLayerMask(unsigned int layerIndex) const;
	IntMask RemoveLayerFromMask(IntMask mask, unsigned int layerIndex);
	IntMask MergeLayerMask(IntMask maskA, IntMask maskB);
	IntMask CreateFullLayerMask();
	IntMask CreateEmptyLayerMask();
	bool ContainsLayer(IntMask mask, unsigned int layerIndex);
	bool AtLeastOneLayerInCommon(IntMask a, IntMask b);
};

#endif
