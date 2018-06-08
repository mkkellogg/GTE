#include "engineobject.h"

namespace GTE {
    EngineObject::EngineObject() : objectID((ObjectID)-1) {

    }

    EngineObject::~EngineObject() {

    }

    ObjectID EngineObject::GetObjectID() const {
        return objectID;
    }

    void EngineObject::SetObjectID(ObjectID id) {
        objectID = id;
    }
}

