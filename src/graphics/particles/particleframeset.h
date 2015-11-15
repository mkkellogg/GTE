/*
* class: ParticleFrameSet
*
* author: Mark Kellogg
*
* This class contains a set of key frames which each contain
* a value for a specified attribute of a particle. For a given
* point in time, this class will interpolate between the two frames
* that most closely correspond to that point in time to determine the
* value for the attribute.
*
*/

#ifndef _GTE_PARTICLE_FRAMESET_H_
#define _GTE_PARTICLE_FRAMESET_H_

#include "object/engineobject.h"
#include "object/enginetypes.h"
#include "particleutil.h"
#include <vector>
#include <string>

namespace GTE
{
	template <typename T> class ParticleFrameSet
	{
		template <typename F> class KeyFrame
		{
			public:

			Real Time;
			F Value;

			KeyFrame(Real time, F value)
			{
				Time = time;
				Value = value;
			}

		};

		std::vector<KeyFrame<T>> frames;

		public:

		ParticleFrameSet()
		{

		}

		virtual ~ParticleFrameSet()
		{

		}

		UInt32 FindNextFrameForTimeValue(Real t)
		{
			UInt32 frameIndex = 0;
			while(frameIndex < frames.size() && frames[frameIndex].Time < t)
			{
				frameIndex = frameIndex + 1;
			}

			return frameIndex;
		}

		Real CalculateFraction(Real a, Real b, Real z)
		{
			return (z - a) / (b - a);
		}

		void AddKeyFrame(Real time, T value)
		{
			KeyFrame<T> newFrame(time, value);
			frames.push_back(newFrame);
		}

		void InterpolateFrameValues(Real t, T& target)
		{
			UInt32 nextFrameIndex = FindNextFrameForTimeValue(t);
			UInt32 currentFrameIndex = nextFrameIndex - 1;
			
			if(nextFrameIndex == 0)
			{
				target = frames[0].Value;
			}
			else if(nextFrameIndex == frames.size())
			{
				target = frames[currentFrameIndex].Value;
			}
			else
			{
				Real fraction = CalculateFraction(frames[currentFrameIndex].Time, frames[nextFrameIndex].Time, t);
				ParticleUtil::Lerp<T>(frames[currentFrameIndex].Value, frames[nextFrameIndex].Value, target, fraction);
			}
		}
	};
}

#endif
