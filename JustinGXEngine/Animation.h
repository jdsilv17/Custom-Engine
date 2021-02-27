#pragma once

#include <vector>
#include <DirectXMath.h>

#include "Object.h"
#include "Time.h"

namespace Animation
{
    struct Joint
    {
        DirectX::XMFLOAT4X4 global_xform;
        DirectX::XMFLOAT4X4 inv_xform;

        int parent_index;

        Object jointObject;

        Joint() : global_xform(), parent_index(0), jointObject() {}
        Joint(DirectX::XMFLOAT4X4 _transform, int _parent)
            : global_xform(_transform), parent_index(_parent), jointObject(_transform) {}
            
    };

    struct Keyframe
    {
        double time = 0;

        std::vector<Joint> joints;
    };

    class Animation
    {
    public:
        Animation() {}

        const double GetAnimationLegnth() const;
        const int GetFrameCount() const;
        std::vector<Keyframe>& GetKeyframes();
        const Keyframe* p_GetKeyframes() const;
        const Keyframe* GetKeyframe(int _index) const;
        const Keyframe* GetCurrentKeyframe() const;
        const Keyframe* AnimationStart() const;
        const Keyframe* AnimationEnd() const;

        void SetAnimationLength(const double& _duration);
        void SetFrameCount(const int& _frameCount);
        void SetKeyframes(const std::vector<Keyframe>& _frames);
        void SetKeyframes(const Keyframe* _frames, size_t numOfKeyframes);
        void SetCurrentFrameTo(int _index);

        const Keyframe* Playback();
        void StartPlayback();
        void StopPlayback();
        void FrameStepForward();
        void FrameStepBack();
        void ResetCurrentFrame();

        bool IsPlaying() const;

        std::vector<Keyframe> Frames;

    private:
        Time Timer;
        double TimeElapsed = 0;
        double Duration = 0;
        int FrameCount = 0;
        int CurrentFrame = 0;
        bool isPlaying = false;
    };
}

