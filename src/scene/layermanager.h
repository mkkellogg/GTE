#ifndef _GTE_LAYERMANAGER_H_
#define _GTE_LAYERMANAGER_H_

#include "engine.h"
#include "base/bitmask.h"

#include <string>
#include <map>


namespace GTE {
    //forward declarations
    class EngineObjectManager;

    class LayerManager {
        friend class EngineObjectManager;

        const UInt32 MAX_LAYERS = 32;
        UInt32 currentLayerIndex;
        std::map<std::string, UInt32> layerIndexes;

        LayerManager();
        ~LayerManager();

    public:

        UInt32 AddLayer(const std::string& name);
        Int32 GetLayerIndex(const std::string& name) const;
        IntMask GetLayerMask(const std::string& name) const;
        IntMask GetLayerMask(UInt32 layerIndex) const;
        IntMask RemoveLayerFromMask(IntMask mask, UInt32 layerIndex);
        IntMask MergeLayerMask(IntMask maskA, IntMask maskB);
        IntMask CreateFullLayerMask();
        IntMask CreateEmptyLayerMask();
        Bool ContainsLayer(IntMask mask, UInt32 layerIndex);
        Bool AtLeastOneLayerInCommon(IntMask a, IntMask b);
    };
}

#endif
