#include "attributetransformer.h"
#include "graphics/stdattributes.h"

namespace GTE {
    AttributeTransformer::AttributeTransformer(StandardAttributeSet attributes) {
        activeAttributes = attributes;
    }

    AttributeTransformer::AttributeTransformer() {
        activeAttributes = StandardAttributes::CreateAttributeSet();
    }

    AttributeTransformer::~AttributeTransformer() {

    }

    void AttributeTransformer::SetActiveAttributes(StandardAttributeSet attributes) {
        activeAttributes = attributes;
    }

    StandardAttributeSet AttributeTransformer::GetActiveAttributes() const {
        return activeAttributes;
    }

    void AttributeTransformer::SetModelMatrix(const Matrix4x4& model, const Matrix4x4& modelInverse) {
        this->model = model;
        this->modelInverse = modelInverse;
    }
}
