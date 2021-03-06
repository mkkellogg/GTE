/*********************************************
*
* class: CrossFadeBlendOp
*
* author: Mark Kellogg
*
* This class is the base class for animation blending operations.
*
***********************************************/

#ifndef _GTE_CROSSFADE_BLENDOP_H_
#define _GTE_CROSSFADE_BLENDOP_H_

#include <vector>
#include <string>
#include <memory>

#include "engine.h"
#include "blendop.h"
#include "global/global.h"

namespace GTE {
    //forward declarations
    class Transform;
    class SkeletonNode;

    class CrossFadeBlendOp : public BlendOp {
        // necessary since AnimationPlayer directly controls this class
        friend class AnimationPlayer;

        UInt32 targetIndex;
        std::function<void(CrossFadeBlendOp*)> startCallback;
        std::function<void(CrossFadeBlendOp*)> completeCallback;
        std::function<void(CrossFadeBlendOp*)> stoppedEarlyCallback;

        CrossFadeBlendOp(Real duration, UInt32 targetIndex);

    protected:

        ~CrossFadeBlendOp()  override;

    public:

        void Update(std::vector<Real>& weights) override;
        void OnStart() override;
        void OnComplete() override;
        void OnStoppedEarly() override;
        void SetOnStartCallback(std::function<void(CrossFadeBlendOp*)> callback);
        void SetOnCompleteCallback(std::function<void(CrossFadeBlendOp*)> callback);
        void SetOnStoppedEarlyCallback(std::function<void(CrossFadeBlendOp*)> callback);
        UInt32 GetTargetIndex() const;
    };
}

#endif
