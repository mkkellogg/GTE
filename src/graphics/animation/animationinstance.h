/*********************************************
*
* class: AnimationInstance
*
* author: Mark Kellogg
*
* This class encapsulates the state of a single instance
* of the playback an Animation object.
*
***********************************************/

#ifndef _GTE_ANIMATION_INSTANCE_H_
#define _GTE_ANIMATION_INSTANCE_H_

#include "engine.h"
#include "animationplayer.h"
#include "global/global.h"

namespace GTE {
    class AnimationInstance {
        friend class EngineObjectManager;
        friend class AnimationManager;
        friend class AnimationPlayer;

    public:

        /*
         * The FrameState class is used to keep track of the state of the animation for
         * each node in the target Skeleton [Target]. The Animation object has a member
         * array [FrameStates] which holds the instance for each node.
         */
        class FrameState {
        public:

            // index of the current translation key for the node
            UInt32 TranslationKeyIndex;
            // index of the current scale key for the node
            UInt32 ScaleKeyIndex;
            // index of the current rotation key for the node
            UInt32 RotationKeyIndex;

            FrameState() {
                TranslationKeyIndex = 0;
                ScaleKeyIndex = 0;
                RotationKeyIndex = 0;
            }

            void Reset() {
                TranslationKeyIndex = 0;
                ScaleKeyIndex = 0;
                RotationKeyIndex = 0;
            }
        };

    private:

        // map each node in [Target] to a channel in [SourceAnimation]
        Int32 * NodeToChannelMap;

        // the target of this instance
        SkeletonSharedPtr Target;
        // the Animation for which this is an instance
        AnimationSharedConstPtr SourceAnimation;
        // used to make animation play faster or slow than default. default = 1.
        Real SpeedFactor;

        // number of states in [FrameStates]. This number should equal the number of nodes in [target].
        // it should also be equal to the number of KeyFrameSet objects in [SourceAnimation].
        UInt32 StateCount;
        // array of FrameState objects, one for each node in [target] and is indexed in the same way.
        // E.g. The FrameState at index 5 corresponds to the SkeletonNode returned by target->GetNode(5);
        FrameState * FrameStates;

        // duration of this instance in seconds
        Real Duration;
        // current progress (in seconds) of this instance
        Real Progress;

        // duration of this instance in ticks
        Real DurationTicks;
        // current progress (in ticks) of this instance
        Real ProgressTicks;

        // we can have the animation start with progress > 0
        Real StartOffset;
        // equivalent of [StartOffset] in ticks
        Real StartOffsetTicks;

        // we can have the animation end earlier than [Duration]
        Real EarlyEnd;
        // equivalent of [EarlyEnd] in ticks
        Real EarlyEndTicks;

        // is the animation playing?
        Bool Playing;

        // is the animation paused?
        Bool Paused;

        // what should happen when playback reaches the end of the clip?
        PlaybackMode PlayBackMode;

        AnimationInstance(SkeletonRef target, AnimationConstRef animation);
        ~AnimationInstance();

        void Destroy();

        void SetSpeed(Real speedFactor);
        void Play();
        void Stop();
        void Pause();

    public:

        Bool Init();
        void Reset();

        FrameState * GetFrameState(UInt32 stateIndex);
        Int32 GetChannelMappingForTargetNode(UInt32 nodeIndex) const;
    };

}
#endif
