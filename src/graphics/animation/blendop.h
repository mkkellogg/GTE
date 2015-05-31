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
#include "object/enginetypes.h"
#include "global/global.h"

namespace GTE
{
	//forward declarations
	class Transform;
	class SkeletonNode;

	class BlendOp
	{
		friend class AnimationPlayer;

	protected:

		Real duration;
		Real progress;
		bool complete;
		bool started;

		std::vector<Real> initialWeights;

		BlendOp(Real duration);
		virtual ~BlendOp();
		void SetComplete(bool complete);
		void SetStarted(bool started);

	public:

		void Reset();
		bool Init(std::vector<Real>& initialWeights);
		virtual void Update(std::vector<Real>& weights);
		virtual void OnStart() = 0;
		virtual void OnComplete() = 0;
		virtual void OnStoppedEarly() = 0;
		bool HasCompleted();
		bool HasStarted();
		Real GetNormalizedProgress();
		Real GetProgress();

	};
}

#endif
