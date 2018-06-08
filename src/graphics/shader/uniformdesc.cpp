#include <memory.h>

#include "uniformdesc.h"
#include "graphics/materialvardirectory.h"

namespace GTE {
    UniformDescriptor::UniformDescriptor() {
        SamplerUnitIndex = 0;
        ShaderVarID = -1;
        Size = -1;
        Type = UniformType::Float;

        RegisteredUniformID = UniformDirectory::VarID_Invalid;
        ExtendedFloatData = nullptr;
        IsSet = false;
        IsDelayedSet = false;
        SetSize = 0;
        RequiresVerification = true;

        memset(BasicFloatData, 0, sizeof(Real) * 4);
    }

    UniformDescriptor::~UniformDescriptor() {

    }
}
