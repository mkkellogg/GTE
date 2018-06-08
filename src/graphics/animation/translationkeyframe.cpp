#include "translationkeyframe.h"
#include "geometry/vector/vector3.h"

namespace GTE {
    /*
    * Default constructor.
    */
    TranslationKeyFrame::TranslationKeyFrame() : KeyFrame() {

    }

    /*
     * Parameterized constructor..
     */
    TranslationKeyFrame::TranslationKeyFrame(Real normalizedTime, Real realTime, Real realTimeTicks, const Vector3& translation) : KeyFrame(normalizedTime, realTime, realTimeTicks) {
        this->Translation = translation;
    }

    /*
     * Destructor.
     */
    TranslationKeyFrame::~TranslationKeyFrame() {

    }
}
