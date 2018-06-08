#include "blendop.h"
#include "util/time.h"

namespace GTE {
    /*
     * Default constructor.
     */
    BlendOp::BlendOp(Real duration) {
        this->duration = duration;
        this->progress = 0;
        complete = false;
        started = false;
    }

    /*
     * Destructor.
     */
    BlendOp::~BlendOp() {

    }

    Bool BlendOp::Init(const std::vector<Real>& initialWeights) {
        this->initialWeights = initialWeights;
        Reset();
        return true;
    }

    void BlendOp::Reset() {
        progress = 0;
        complete = false;
    }

    void BlendOp::Update(std::vector<Real>& weights) {
        progress += Time::GetDeltaTime();
    }

    void BlendOp::SetStarted(Bool started) {
        this->started = started;
    }

    void BlendOp::SetComplete(Bool complete) {
        this->complete = complete;
    }

    Bool BlendOp::HasStarted() const {
        return started;
    }

    Bool BlendOp::HasCompleted() const {
        return complete;
    }

    Real BlendOp::GetNormalizedProgress() const {
        return progress / duration;
    }

    Real BlendOp::GetProgress() const {
        return progress;
    }
}

