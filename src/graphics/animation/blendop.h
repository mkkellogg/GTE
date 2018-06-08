/*********************************************
*
* class: BlendOp
*
* author: Mark Kellogg
*
* This class is the base class for animation blending operations.
*
***********************************************/

#ifndef _GTE_BLENDOP_H_
#define _GTE_BLENDOP_H_

#include <vector>
#include <string>

#include "engine.h"
#include "global/global.h"

namespace GTE {
    //forward declarations
    class Transform;
    class SkeletonNode;

    class BlendOp {
        friend class AnimationPlayer;

    protected:

        Real duration;
        Real progress;
        Bool complete;
        Bool started;

        std::vector<Real> initialWeights;

        BlendOp(Real duration);
        virtual ~BlendOp();

        void SetComplete(Bool complete);
        void SetStarted(Bool started);

    public:

        void Reset();
        Bool Init(const std::vector<Real>& initialWeights);
        virtual void Update(std::vector<Real>& weights);
        virtual void OnStart() = 0;
        virtual void OnComplete() = 0;
        virtual void OnStoppedEarly() = 0;
        Bool HasCompleted() const;
        Bool HasStarted() const;
        Real GetNormalizedProgress() const;
        Real GetProgress() const;

    };
}

#endif
